#!/usr/bin/env python3
"""Bounded CH32V303 diagnostics; no Flash dump, reset, programming or unlock."""
import argparse
import datetime as dt
import json
from pathlib import Path
import secrets
import sys
import time

def exchange(port, command, event, timeout=10, request=None, trace=None):
    # Ignore boot text and unrelated events. Malformed matching JSON is an error.
    port.write((command + "\n").encode("ascii"))
    port.flush()
    deadline = time.monotonic() + timeout
    pending = b""
    while time.monotonic() < deadline:
        chunk = port.readline()
        if not chunk:
            continue
        pending += chunk
        while b"\n" in pending:
            line, pending = pending.split(b"\n", 1)
            try:
                msg = json.loads(line.decode("utf-8"))
            except (UnicodeError, json.JSONDecodeError):
                continue
            if (isinstance(msg, dict) and msg.get("event") == "dmi"
                    and msg.get("request") == request and trace is not None):
                trace.append(msg)
                print(json.dumps(msg), file=sys.stderr)
            if isinstance(msg, dict) and msg.get("event") == event:
                if request is None or msg.get("request") == request:
                    return msg
            if request is not None and isinstance(msg, dict) and msg.get("event") == "info":
                raise RuntimeError("Adapter restarted during target operation; completion unconfirmed; no retry")
            if isinstance(msg, dict) and msg.get("event") == "error":
                raise RuntimeError(f"Adapter rejected command: {msg}")
        if len(pending) > 8192:
            pending = b""  # discard an unterminated boot/noise line
    raise TimeoutError(f"No {event!r} reply within {timeout}s; no automatic retry")


def synchronize_info(port, timeout=10):
    # A reset can consume the first command before app_main starts. Only the
    # GPIO-passive hello handshake is retried; identify is always sent once.
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        try:
            return exchange(port, "\nhello", "info",
                            timeout=min(1.0, deadline - time.monotonic()))
        except TimeoutError:
            pass
    raise TimeoutError("UART startup synchronization failed within 10s")


def decode_status(result):
    """Keep raw evidence alongside named fields; unavailable words remain untrusted."""
    status = int(result.get("dmstatus", "0"), 16)
    control = int(result.get("dmcontrol", "0"), 16)
    names = {7: "authenticated", 6: "authbusy", 8: "anyhalted", 9: "allhalted",
             10: "anyrunning", 11: "allrunning", 12: "anyunavail", 13: "allunavail",
             14: "anynonexistent", 15: "allnonexistent", 16: "anyresumeack",
             17: "allresumeack", 18: "anyhavereset", 19: "allhavereset"}
    return {"valid_stable_preflight": (result.get("completed_samples", 0) >= 3 and
                    (result.get("capabilities", {}).get("dmi", {}).get("status") == "supported"
                     or result.get("result") == "ok")),
            "dmstatus": {"version": status & 15,
                         **{name: bool(status & (1 << bit)) for bit, name in names.items()}},
            "dmcontrol": {"dmactive": bool(control & 1), "ndmreset": bool(control & 2),
                          "hartreset": bool(control & (1 << 29)),
                          "hartsel": ((control >> 16) & 1023) | (((control >> 6) & 1023) << 10),
                          "hasel": bool(control & (1 << 26)),
                          "haltreq_resumereq_readback": "write_only_not_action_evidence"}}


def selection(args):
    return sum(bit for name, bit in (("registers", 1), ("sram", 2), ("gpio", 4),
                                    ("usart", 8), ("spi", 16)) if getattr(args, name, False))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", required=True, help="Serial device matching the firmware console")
    parser.add_argument("--output", type=Path, required=True, help="New JSON record; refuses overwrite")
    sub = parser.add_subparsers(dest="command", required=True)
    sub.add_parser("info")
    for name in ("identify", "diagnose", "capabilities", "activate-dm", "halt-cycle"):
        action = sub.add_parser(name)
        action.add_argument("--voltage-mv", type=int, required=True,
                            help="Fresh external meter measurement at header pin4 to pin1")
        action.add_argument("--board-pins-checked", action="store_true", required=True)
        action.add_argument("--wiring-checked", action="store_true", required=True)
        if name in ("capabilities", "halt-cycle"):
            for flag in ("registers", "sram", "gpio", "usart", "spi"):
                action.add_argument(f"--{flag}", action="store_true",
                                    help="Explicit bounded snapshot; never implicitly halts")
        if name == "activate-dm":
            action.add_argument("--allow-dm-activation", action="store_true", required=True,
                                help="Explicit DM activation; also requires its firmware build gate")
        if name == "halt-cycle":
            action.add_argument("--allow-halt-resume", action="store_true", required=True,
                                help="Explicit halt/resume experiment; also requires its firmware build gate")
    args = parser.parse_args()
    if args.command != "info" and not 3000 <= args.voltage_mv <= 3450:
        parser.error("Measured target rail must be 3000..3450 mV for this 3.3V setup")
    if args.output.exists():
        parser.error("Output exists; choose a new capture filename")
    import serial  # pyserial; imported only after validating CLI arguments

    result = None
    record = {"utc": dt.datetime.now(dt.timezone.utc).isoformat(),
              "port": args.port, "command": args.command,
              "target_validated": False, "memory_read": False}
    # Prepare modem lines before opening: do not deliberately reset the ESP32.
    port = serial.Serial(port=None, baudrate=115200, timeout=0.25, write_timeout=2,
                         exclusive=True)
    port.dtr = False
    port.rts = False
    port.port = args.port
    try:
        port.open()
        port.reset_input_buffer()
        info = synchronize_info(port)
        record["adapter"] = info
        if info.get("protocol") != 3 or info.get("build") != "read-only-diagnostic":
            raise RuntimeError("Unexpected firmware protocol/build")
        if (info.get("target_memory_write") is not False or info.get("flash_write") is not False
                or info.get("memory_read") is not True or info.get("dmi_write") is not True
                or info.get("flash_dump") is not False or info.get("reset_command") is not False):
            raise RuntimeError("Adapter does not declare the required read-only policy")
        if args.command != "info":
            if (info.get("diagnostic_modes") is not True or info.get("identify_writes") is not False
                    or info.get("dmactive_requires_explicit_command") is not True):
                raise RuntimeError("Expected firmware with separate read-only identification and explicit DM activation")
            if (info.get("adapter_target") != "esp32" or info.get("console") != "uart0"
                    or (info.get("gpio_clk"), info.get("gpio_dio")) != (25, 26)):
                raise RuntimeError("Expected classic ESP32 UART0 firmware with SWCLK=GPIO25, SWDIO=GPIO26")
            if not info.get("board_verified") or not info.get("initialized"):
                raise RuntimeError("Board is unverified or GPIO setup failed; see README")
            record["operator_voltage_mv"] = args.voltage_mv
            if args.command == "activate-dm" and info.get("dm_activation_enabled") is not True:
                raise RuntimeError("DM activation disabled by firmware build policy")
            if args.command == "halt-cycle" and info.get("halt_resume_enabled") is not True:
                raise RuntimeError("Halt/resume disabled by firmware build policy")
            request = secrets.token_hex(8)
            command = f"{args.command} {request} {args.voltage_mv} PINS25_26_WIRING_CHECKED"
            if args.command == "activate-dm":
                command += " EXPLICIT_DM_ACTIVATION"
            if args.command == "halt-cycle":
                command += " EXPLICIT_HALT_RESUME"
            if args.command in ("capabilities", "halt-cycle"):
                command += f" {selection(args)}"
            record["dmi_trace"] = []
            result = exchange(port, command, args.command, request=request,
                              timeout=60, trace=record["dmi_trace"])
            key = ("diagnostic" if args.command in ("capabilities", "halt-cycle") else
                   "activation" if args.command == "activate-dm" else "identification")
            record[key] = result
            record["decoded"] = decode_status(result)
            record["memory_read"] = bool(result.get("snapshots"))
            if result.get("trace_overflow") is True:
                raise RuntimeError("Incomplete DMI trace: adapter buffer overflow")
            if result.get("pins_released") is False or result.get("target_state_uncertain") is True:
                raise RuntimeError("Adapter cleanup or target running state is unconfirmed; no automatic action")
            if args.command == "activate-dm" and (result.get("activation_state_uncertain") is True or
                    result.get("pins_released") is not True):
                raise RuntimeError("DM activation/readback or pin release is unconfirmed; do not retry automatically")
        else:
            result = info
    except (OSError, RuntimeError, TimeoutError, ValueError, TypeError) as exc:
        record["error"] = str(exc)
    except KeyboardInterrupt:
        record["error"] = "Interrupted by operator; diagnostic was not retried"
    finally:
        port.close()
    # One complete evidence record, even on a protocol/transport error.
    args.output.parent.mkdir(parents=True, exist_ok=True)
    with args.output.open("x", encoding="utf-8") as stream:
        json.dump(record, stream, indent=2)
        stream.write("\n")
    print(json.dumps(record, indent=2))
    if "error" in record or (args.command != "info" and result.get("result") != "ok"):
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
