import argparse
from datetime import datetime
import fcntl
import json
import math
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import time
import uuid
import zipfile
from zoneinfo import ZoneInfo

from .protocol import ROOT
from .session import analyze, sha256, write_json

TESTS = {
    "startup_idle": "Ölçüm girişleri boş. DMT99 kapalı; kayıt başlayınca açın.",
    "idle": "DMT99 zaten açık; mevcut bağlantı ve modu kayıt boyunca sabit tutun.",
    "resistance_open": "Direnç modu; problar açık uç.",
    "resistance_reference": "Direnç modu; kullanıcının bildirdiği referans kartı bağlantısını sabit tutun.",
    "resistance_1k": "Direnç modu; enerjisiz 1 kohm direnç COM ve V/ohm girişleri arasında.",
    "resistance_10k": "Direnç modu; enerjisiz 10 kohm direnç COM ve V/ohm girişleri arasında.",
    "resistance_100k": "Direnç modu; enerjisiz 100 kohm direnç COM ve V/ohm girişleri arasında.",
    "capacitance_100n": "Kapasitans modu; boşaltılmış 100 nF kondansatör.",
    "capacitance_1u": "Kapasitans modu; boşaltılmış 1 uF kondansatör.",
    "capacitance_10u": "Kapasitans modu; boşaltılmış 10 uF kondansatör; varsa polariteye dikkat edin.",
    "dc_battery_1v5": "DC V modu; yalnızca 1,5 V pil. Eksi COM, artı V/ohm.",
    "continuity_open": "Süreklilik modu; problar açık.",
    "continuity_closed": "Süreklilik modu; problar birbirine değiyor; harici enerji yok.",
    "auto_transitions": "AUTO: açık uç -> enerjisiz direnç -> açık uç -> 1,5 V pil; zamanları not edin.",
    "mode_transition": "Mod geçişi kaydı; kayıt etkin bildirildikten sonra istenen modu seçin.",
    "open_mode_sweep": "Problar açık; harici komponent/kaynak bağlamadan kullanıcı mod geçişleri yapacak.",
    "capacitance_open": "Kapasitans modu, problar açık; kondansatör bağlanmayacak.",
    "voltage_dc_open": "DC voltaj modu; problar açık, harici kaynak yok.",
    "voltage_ac_open": "AC voltaj modu; problar açık, harici kaynak yok.",
    "frequency_open": "Frekans modu; problar açık, sinyal kaynağı yok.",
    "current_dc_open": "DC amper modu; ölçüm girişlerine hiçbir şey bağlı değil.",
    "current_ac_open": "AC amper modu; ölçüm girişlerine hiçbir şey bağlı değil.",
    "current_dc_ma_open": "DC mA modu; ölçüm girişlerine hiçbir şey bağlı değil.",
    "current_ma_open": "mA modu, DC/AC henüz kullanıcı tarafından belirtilmedi; girişler boş.",
    "current_a_open": "A modu, DC/AC ayrıca metadata'da belirtilir; girişler boş.",
    "current_ac_ma_open": "AC mA modu; ölçüm girişlerine hiçbir şey bağlı değil.",
    "ncv_idle": "NCV modu; cihaz masada, enerjili kablolara yaklaştırılmayacak.",
    "live_idle": "LIVE modu; prob boşta, kabloya veya gerilim kaynağına temas yok.",
    "temperature_open": "Sıcaklık modu; harici sıcaklık probu/sensörü bağlı değil.",
}


def now():
    return datetime.now(ZoneInfo("Europe/Istanbul")).isoformat()


def environment():
    env = os.environ.copy()
    local = ROOT / ".local/extracted/usr/share"
    if (local / "sigrok-firmware/fx2lafw-saleae-logic.fw").is_file():
        env["XDG_DATA_HOME"] = str(local)
    return env


def run_readonly(args, timeout=15):
    try:
        p = subprocess.run(args, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                           timeout=timeout, env=environment())
        return {"argv": args, "returncode": p.returncode, "output": p.stdout}
    except (OSError, subprocess.TimeoutExpired) as exc:
        return {"argv": args, "returncode": None, "output": str(exc)}


def device_info():
    return run_readonly(["sigrok-cli", "--driver", "fx2lafw", "--show"])


def accessible(info):
    text = info["output"]
    return (info["returncode"] == 0 and "Supported configuration options" in text and
            "samplerate" in text and not re.search(r"Failed|Unable|No devices found", text))


def probe(_args):
    report = {"timestamp": now(), "lsusb": run_readonly(["lsusb"]),
              "sigrok_version": run_readonly(["sigrok-cli", "--version"]),
              "pulseview_binary": shutil.which("pulseview"), "driver": device_info(),
              "packages": run_readonly(["rpm", "-q", "sigrok-cli", "pulseview", "libsigrok",
                                         "libsigrokdecode", "sigrok-firmware-fx2lafw"]),
              "local_analyzer_firmware": str(ROOT / ".local/extracted/usr/share/sigrok-firmware"),
              "install_command": "sudo dnf install sigrok-cli pulseview sigrok-firmware-fx2lafw"}
    report["accessible"] = accessible(report["driver"])
    stamp = datetime.now(ZoneInfo("Europe/Istanbul")).strftime("%Y%m%dT%H%M%S%f")
    path = ROOT / "evidence" / f"probe_{stamp}.json"
    write_json(path, report)
    print(report["lsusb"]["output"])
    print(report["driver"]["output"])
    print(f"Analyzer erişilebilir: {report['accessible']}\nKanıt: {path}")
    return 0 if report["accessible"] else 2


def metadata_from(args):
    return {"test": args.test, "label": args.label or args.test,
            "display_text": args.display, "display_numeric": args.display_numeric,
            "display_unit": args.display_unit, "display_tolerance": args.tolerance,
            "display_is_approximate": bool(args.display_approximate),
            "component_actual": args.actual, "mode": args.mode, "notes": args.notes}


def create_directory(label):
    safe = re.sub(r"[^A-Za-z0-9_-]", "_", label)[:64] or "capture"
    stamp = datetime.now(ZoneInfo("Europe/Istanbul")).strftime("%Y%m%dT%H%M%S")
    path = ROOT / "captures" / f"{stamp}_{safe}_{uuid.uuid4().hex[:8]}"
    path.mkdir(parents=True, exist_ok=False)
    return path


def capture(args):
    if not args.connected:
        raise ValueError("Fiziksel bağlantıyı doğruladıktan sonra --connected kullanın")
    if args.samplerate not in (1000000, 2000000):
        raise ValueError("Bu başlangıç profili yalnızca 1 veya 2 MHz kullanır")
    if args.tx_channel == args.rx_channel:
        raise ValueError("TX and RX must use different channels")
    if not math.isfinite(args.seconds) or args.seconds <= 0:
        raise ValueError("seconds must be finite and positive")
    lockpath = ROOT / ".local/capture.lock"
    lockpath.parent.mkdir(exist_ok=True)
    with lockpath.open("w") as lock:
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        info = device_info()
        if not accessible(info):
            raise ValueError("Analyzer açılamadı. PulseView'i kapatıp probe çalıştırın.\n" + info["output"])
        directory = create_directory(args.label or args.test)
        samples = math.ceil(args.seconds * args.samplerate)
        command = ["sigrok-cli", "--loglevel", "4", "--driver", "fx2lafw",
                   "--config", f"samplerate={args.samplerate}", "--channels", f"D{args.tx_channel},D{args.rx_channel}",
                   "--samples", str(samples), "--output-format", "srzip",
                   "--output-file", str(directory / "raw.sr")]
        metadata = dict(metadata_from(args), schema=1, origin="hardware", status="starting",
                        host_started_at=now(), requested_seconds=args.seconds,
                        requested_samples=samples, requested_samplerate_hz=args.samplerate,
                        channels={f"D{args.tx_channel}": "MCU_TX_PA9_pin42", f"D{args.rx_channel}": "ASIC_TX_PA10_pin43"},
                        channel_map={"MCU_TX": args.tx_channel, "ASIC_TX": args.rx_channel},
                        rx_frame_bytes=args.rx_frame_bytes,
                        uart="9600 8N1", passive_connections_confirmed=True,
                        device_preflight=info, command=command)
        write_json(directory / "metadata.json", metadata)
        print(TESTS[args.test], flush=True)
        print(f"Pasif kanal profili: D{args.tx_channel}=MCU_TX, D{args.rx_channel}=ASIC_TX", flush=True)
        print(f"Kayıt komutu başlatılıyor: {directory}\nSüre: {args.seconds}s", flush=True)
        code = None
        with (directory / "sigrok.log").open("w") as log:
            process = subprocess.Popen(command, stdout=log, stderr=subprocess.STDOUT, env=environment())
            try:
                deadline = time.monotonic() + args.seconds + 20
                notified = False
                while process.poll() is None:
                    if not notified and (directory / "raw.sr").exists() and (directory / "raw.sr").stat().st_size:
                        print("Ham veri dosyasına yazılıyor; kayıt etkin.", flush=True)
                        notified = True
                    if time.monotonic() > deadline:
                        metadata["timeout"] = True
                        process.terminate()
                        break
                    time.sleep(0.1)
                code = process.wait(timeout=5)
            except (KeyboardInterrupt, subprocess.TimeoutExpired):
                metadata["interrupted"] = True
                process.terminate()
                try: code = process.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    process.kill(); code = process.wait()
            finally:
                metadata.update(host_finished_at=now(), returncode=code, status="acquisition_finished")
                write_json(directory / "metadata.json", metadata)
        try:
            result = analyze(directory)
            count = result["sample_count"]
            metadata["received_samples"] = count
            metadata["status"] = "captured" if code == 0 and count >= samples else "partial_or_failed"
            metadata["analysis_error"] = None
        except (ValueError, OSError, KeyError, zipfile.BadZipFile) as exc:
            metadata["status"] = "partial_or_failed"
            metadata["analysis_error"] = str(exc)
            print(f"Analiz tamamlanamadı; ham dosyalar korundu: {exc}", file=sys.stderr)
        write_json(directory / "metadata.json", metadata)
        # Final status does not require decoding a second time.
        if metadata["analysis_error"] is None:
            result["metadata"] = metadata
            write_json(directory / "decoded.json", result)
        print(f"Durum: {metadata['status']}\nOturum: {directory}", flush=True)
        return 0 if metadata["status"] == "captured" else 2


def annotate(args):
    path = Path(args.directory) / "metadata.json"
    metadata = json.loads(path.read_text())
    previous = {k: metadata.get(k) for k in ("display_text", "display_numeric", "display_unit",
                                              "display_tolerance", "display_is_approximate", "component_actual", "mode", "notes")}
    updates = {"display_text": args.display, "display_numeric": args.display_numeric,
               "display_unit": args.display_unit, "display_tolerance": args.tolerance,
               "component_actual": args.actual, "mode": args.mode, "notes": args.notes}
    updates["display_is_approximate"] = args.display_approximate
    metadata.setdefault("annotation_history", []).append({"timestamp": now(), "previous": previous})
    metadata.update({k: v for k, v in updates.items() if v is not None})
    write_json(path, metadata)
    analyze(args.directory)
    return 0


def imported(args):
    directory = create_directory(args.label or args.test)
    shutil.copyfile(args.srfile, directory / "raw.sr")
    write_json(directory / "metadata.json", dict(metadata_from(args), schema=1,
               origin="hardware" if args.hardware else "imported_unverified",
               channel_map={"MCU_TX": args.tx_channel, "ASIC_TX": args.rx_channel},
               rx_frame_bytes=args.rx_frame_bytes,
               imported_at=now(), imported_from=str(Path(args.srfile).resolve()),
               imported_sha256=sha256(args.srfile), status="imported"))
    analyze(directory)
    print(directory)
    return 0


def add_metadata(parser, default_tolerance=None):
    parser.add_argument("--display", help="Ekranda görülen tam metin, ör. '0.999 kΩ' veya 'OL'")
    parser.add_argument("--display-numeric", type=finite_float)
    parser.add_argument("--display-unit", help="V,mV,uA,mA,A,ohm,kohm,Mohm,nF,uF,mF,Hz,kHz,MHz")
    parser.add_argument("--tolerance", type=nonnegative_float, default=default_tolerance)
    precision = parser.add_mutually_exclusive_group()
    precision.add_argument("--display-approximate", dest="display_approximate", action="store_true", default=None)
    precision.add_argument("--display-exact", dest="display_approximate", action="store_false")
    parser.add_argument("--actual", help="Gerçek/nominal komponent değeri ve toleransı; serbest metin")
    parser.add_argument("--mode")
    parser.add_argument("--notes")


def finite_float(value):
    number = float(value)
    if not math.isfinite(number): raise argparse.ArgumentTypeError("finite number required")
    return number


def nonnegative_float(value):
    number = finite_float(value)
    if number < 0: raise argparse.ArgumentTypeError("nonnegative number required")
    return number


def main():
    parser = argparse.ArgumentParser(description="DMT99 pasif ASIC UART kaydı ve analizi")
    subs = parser.add_subparsers(dest="action", required=True)
    subs.add_parser("probe").set_defaults(handler=probe)
    scenarios = subs.add_parser("scenarios")
    scenarios.set_defaults(handler=lambda _: print(json.dumps(TESTS, indent=2, ensure_ascii=False)) or 0)
    cap = subs.add_parser("capture")
    cap.add_argument("--test", choices=TESTS, required=True)
    cap.add_argument("--label")
    cap.add_argument("--seconds", type=finite_float, default=20)
    cap.add_argument("--samplerate", type=int, default=1000000)
    cap.add_argument("--connected", action="store_true", help="GND ve seçili TX/RX kanallarının pasif bağlantısı hazır")
    cap.add_argument("--tx-channel", type=int, choices=range(8), default=0)
    cap.add_argument("--rx-channel", type=int, choices=range(8), default=2)
    cap.add_argument("--rx-frame-bytes", type=int, choices=(12, 13), default=12,
                     help="RX length hypothesis; stock DMA length is 13; AUTO hardware burst observed 12")
    add_metadata(cap, 0)
    cap.set_defaults(handler=capture)
    dec = subs.add_parser("analyze")
    dec.add_argument("directory")
    dec.add_argument("--gap-chars", type=finite_float, default=4)
    dec.set_defaults(handler=lambda a: analyze(a.directory, gap_chars=a.gap_chars) and 0)
    ann = subs.add_parser("annotate")
    ann.add_argument("directory")
    add_metadata(ann)
    ann.set_defaults(handler=annotate)
    imp = subs.add_parser("import-sr")
    imp.add_argument("srfile")
    imp.add_argument("--hardware", action="store_true", help="Bu dosyanın gerçek donanım kaydı olduğunu belirtir")
    imp.add_argument("--test", choices=TESTS, required=True)
    imp.add_argument("--label")
    imp.add_argument("--tx-channel", type=int, choices=range(8), default=0)
    imp.add_argument("--rx-channel", type=int, choices=range(8), default=2)
    imp.add_argument("--rx-frame-bytes", type=int, choices=(12, 13), default=12)
    add_metadata(imp, 0)
    imp.set_defaults(handler=imported)
    comparison = subs.add_parser("compare")
    comparison.add_argument("directories", nargs="+")
    comparison.add_argument("--output", default=str(ROOT / "evidence/comparison.json"))
    def do_compare(args):
        from .analysis import compare
        results = [json.loads((Path(p) / "decoded.json").read_text()) for p in args.directories]
        write_json(args.output, compare(results))
        print(args.output)
        return 0
    comparison.set_defaults(handler=do_compare)
    args = parser.parse_args()
    try: return args.handler(args)
    except (ValueError, OSError, KeyError, zipfile.BadZipFile) as exc:
        print(f"Hata: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
