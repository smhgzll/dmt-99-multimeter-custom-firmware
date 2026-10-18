import importlib.util
import contextlib
import hashlib
import io
import json
import os
from pathlib import Path
import select
import subprocess
import sys
import tempfile
import threading
import unittest

ROOT = Path(__file__).resolve().parents[1]


def module(name):
    spec = importlib.util.spec_from_file_location(name, ROOT / "tools" / f"{name}.py")
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


class FakePort:
    def __init__(self, lines):
        self.lines = iter(lines)
        self.sent = []

    def write(self, b):
        self.sent.append(b)

    def flush(self):
        pass

    def readline(self):
        return next(self.lines, b"")


class HostTests(unittest.TestCase):
    @unittest.skipUnless(importlib.util.find_spec("serial") and os.name == "posix",
                         "pyserial and POSIX pseudoterminals required")
    def test_cli_serial_capture_on_synthetic_pty(self):
        # Exercises the actual CLI and pyserial. No adapter/target is contacted.
        master, slave = os.openpty()
        commands = []

        def peer():
            pending = b""
            while len(commands) < 3:
                while b"\n" not in pending:
                    if not select.select([master], [], [], 5)[0]:
                        return
                    pending += os.read(master, 4096)
                line, pending = pending.split(b"\n", 1)
                command = line.decode().strip()
                if not command:
                    continue
                commands.append(command)
                if command == "hello":
                    if commands.count("hello") == 1:
                        continue  # simulate the first hello being lost during ESP32 reset
                    msg = {"event": "info", "protocol": 3, "build": "read-only-diagnostic",
                           "flash_dump": False, "reset_command": False,
                           "memory_read": True, "dmi_write": True,
                           "target_memory_write": False, "flash_write": False,
                           "diagnostic_modes": True, "identify_writes": False,
                           "dmactive_requires_explicit_command": True,
                           "board_verified": True, "initialized": True,
                           "adapter_target": "esp32", "console": "uart0",
                           "gpio_clk": 25, "gpio_dio": 26}
                elif command.startswith("identify "):
                    trace = {"event": "dmi", "request": command.split()[1],
                             "sequence": 1, "address": "7f", "operation": "read",
                             "result": "no_response", "stage": "chip_id_check"}
                    os.write(master, (json.dumps(trace) + "\n").encode())
                    msg = {"event": "identify", "request": command.split()[1],
                           "result": "no_response", "memory_read": False}
                else:
                    return
                os.write(master, (json.dumps(msg) + "\n").encode())

        thread = threading.Thread(target=peer, daemon=True)
        try:
            with tempfile.TemporaryDirectory() as tmp:
                output = Path(tmp) / "synthetic-capture.json"
                thread.start()
                proc = subprocess.run([sys.executable, str(ROOT / "tools/diag.py"),
                    "--port", os.ttyname(slave), "--output", str(output), "identify",
                    "--voltage-mv", "3300", "--board-pins-checked", "--wiring-checked"],
                    capture_output=True, timeout=15)
                thread.join(timeout=1)
                self.assertEqual(proc.returncode, 1, proc.stderr.decode())
                record = json.loads(output.read_text())
                self.assertEqual(record["identification"]["result"], "no_response")
                self.assertFalse(record["memory_read"])
                self.assertEqual(record["dmi_trace"][0]["address"], "7f")
                self.assertEqual(len(commands), 3)
                self.assertEqual(commands[0], "hello")
                self.assertEqual(commands[1], "hello")
                self.assertEqual(commands[2].split()[2:],
                                 ["3300", "PINS25_26_WIRING_CHECKED"])
        finally:
            os.close(master)
            os.close(slave)

    def test_exchange_noise_and_expected_event(self):
        port = FakePort([b"boot ROM\n", b"\xff\n", b"[]\n",
                         b'{"event":"info"}\n', b'{"event":"identify",', b'"result":"ok"}\n'])
        msg = module("diag").exchange(port, "identify 3300 PINS25_26_WIRING_CHECKED", "identify")
        self.assertEqual(msg["result"], "ok")
        self.assertEqual(len(port.sent), 1)

    def test_exchange_error_never_retries(self):
        port = FakePort([b'{"event":"error","result":"unsupported_command"}\n'])
        with self.assertRaises(RuntimeError):
            module("diag").exchange(port, "hello", "info")
        self.assertEqual(port.sent, [b"hello\n"])

    def test_dmi_trace_is_retained_before_parity_error(self):
        trace = [{"event": "dmi", "request": "fresh", "sequence": 1,
                  "address": "7f", "operation": "read", "result": "ok"},
                 {"event": "dmi", "request": "fresh", "sequence": 2,
                  "address": "10", "operation": "read", "word": "ffffffff",
                  "parity_expected": 0, "parity_received": 1,
                  "result": "parity_error", "stage": "data_parity_check"}]
        lines = [(json.dumps(t) + "\n").encode() for t in trace]
        lines.append(b'{"event":"diagnose","request":"fresh","result":"parity_error"}\n')
        port, saved = FakePort(lines), []
        with contextlib.redirect_stderr(io.StringIO()):
            reply = module("diag").exchange(port, "diagnose fresh", "diagnose",
                                            request="fresh", trace=saved)
        self.assertEqual(reply["result"], "parity_error")
        self.assertEqual(saved, trace)
        self.assertEqual(port.sent, [b"diagnose fresh\n"])

    def test_exchange_timeout_never_retries(self):
        port = FakePort([])
        with self.assertRaises(TimeoutError):
            module("diag").exchange(port, "hello", "info", timeout=0.001)
        self.assertEqual(port.sent, [b"hello\n"])

    def test_stale_identification_is_ignored(self):
        port = FakePort([b'{"event":"identify","request":"old"}\n',
                         b'{"event":"identify","request":"fresh","result":"ok"}\n'])
        msg = module("diag").exchange(port, "command", "identify", request="fresh")
        self.assertEqual(msg["request"], "fresh")
        self.assertEqual(len(port.sent), 1)

    def test_voltage_argument_before_serial_open(self):
        proc = subprocess.run([sys.executable, str(ROOT / "tools/diag.py"),
                               "--port", "/not/a/device", "--output", "/tmp/unused-dmt99.json",
                               "identify", "--voltage-mv", "5000", "--board-pins-checked",
                               "--wiring-checked"], capture_output=True)
        self.assertEqual(proc.returncode, 2)
        self.assertIn(b"3000..3450", proc.stderr)

    def test_dumps_exact_size_match_mismatch_same_file(self):
        script = ROOT / "tools/compare_dumps.py"
        with tempfile.TemporaryDirectory() as tmp:
            a, b = Path(tmp) / "a.bin", Path(tmp) / "b.bin"
            a.write_bytes(bytes(range(256)) * 1024)
            b.write_bytes(a.read_bytes())

            def run(x, y):
                return subprocess.run([sys.executable, str(script), str(x), str(y)],
                                      capture_output=True, text=True)

            p = run(a, b)
            self.assertEqual(p.returncode, 0)
            self.assertTrue(json.loads(p.stdout)["match"])
            b.write_bytes(b"X" + a.read_bytes()[1:])
            self.assertEqual(run(a, b).returncode, 1)
            b.write_bytes(b"short")
            self.assertEqual(run(a, b).returncode, 2)
            self.assertEqual(run(a, a).returncode, 2)


if __name__ == "__main__":
    unittest.main()
