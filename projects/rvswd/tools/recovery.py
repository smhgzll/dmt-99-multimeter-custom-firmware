#!/usr/bin/env python3
"""Bounded recovery investigation. Connect mode sends only adapter hello.

The unvalidated Abstract Memory experiment is restricted to an explicitly
selected spare or separately authorized original running/halt-cycle target. Never flashes.
"""
import argparse
import datetime as dt
import hashlib
import json
from pathlib import Path
import secrets
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
import diag

WORD_PROFILES = {'prefix': (1, 0x08000000), 'boot-alias': (2, 0), 'app-alias': (3, 0x6400), 'sram': (4, 0x20000000)}
ADDRESSES = [0x08006400 + i * 4 for i in range(4)] + [0x08000000 + i * 4 for i in range(4)]


def digest(data):
    return hashlib.sha256(data).hexdigest()


def file_evidence(path):
    data = path.read_bytes()
    return {'path': str(path.resolve()), 'length': len(data), 'sha256': digest(data)}


def audit(captures, reference):
    records = []
    sba_zero_evidence = []
    for path in sorted(captures.rglob('*')):
        if not path.is_file():
            continue
        item = file_evidence(path)
        if path.suffix == '.json':
            try:
                record = json.loads(path.read_text())
                item['command'] = record.get('command')
                result = record.get('diagnostic', record.get('activation', record.get('identification', {})))
                item['observations'] = {k: result[k] for k in (
                    'result', 'chip_id', 'dmcontrol', 'dmstatus', 'abstractcs',
                    'abstractauto', 'abstractauto_valid', 'sbcs', 'sbcs_valid',
                    'completed_samples', 'pins_released', 'trace_overflow') if k in result}
                if record.get('error'):
                    item['recorded_error'] = record['error']
                if (record.get('command') == 'capabilities' and result.get('result') == 'ok'
                        and result.get('sbcs_valid') is True and result.get('sbcs') == '00000000'
                        and result.get('pins_released') is True and result.get('trace_overflow') is False
                        and result.get('completed_samples', 0) >= 3):
                    sba_zero_evidence.append(str(path))
                if isinstance(record.get('dump'), dict):
                    item['historical_dump_result'] = record['dump']
            except (ValueError, AttributeError) as exc:
                item['parse_error'] = str(exc)
        records.append(item)
    return {'existing_evidence': records, 'reference': file_evidence(reference),
            'reference_base': '08006400', 'reference_is_original_target_capture': False,
            'missing_prefix': {'address': '08000000', 'length': 25600},
            'recovery_verified': False,
            'decision': 'spare_validation_required',
            'sba_zero_evidence': sba_zero_evidence,
            'methods': [
                {'method': 'sba', 'decision': 'skip_known_sbcs_zero_no_new_scan' if sba_zero_evidence else 'unconfirmed_no_flash_route_implemented'},
                {'method': 'abstract_access_memory', 'decision': 'lab_only_documentation_conflict'},
                {'method': 'progbuf', 'decision': 'requires_explicit_progbuf_mode'},
                {'method': 'rom_isp', 'decision': 'no_verified_code_flash_readout_and_boot_change_required'},
                {'method': 'aliases', 'decision': 'no_independent_read_transport_or_rdp_exception'},
            ]}


def validate_adapter(info, lab=False, running=False, cycle=False, progbuf=False):
    expected = {'protocol': 3, 'build': 'read-only-diagnostic', 'adapter_target': 'esp32',
                'console': 'uart0', 'gpio_clk': 25, 'gpio_dio': 26,
                'initialized': True, 'board_verified': True, 'target_memory_write': False,
                'flash_write': False, 'reset_command': False, 'identify_writes': False,
                'halt_resume_enabled': cycle}
    if progbuf:
        expected['progbuf_lab'] = True
    if cycle:
        expected['halt_memory_cycle'] = True
    if lab:
        expected.update(abstract_memory_lab=True, memory_lab_requires_halted=True,
                        abstract_memory_running_validated=False)
    if running:
        expected['running_spare_lab'] = True
    if any(type(info.get(k)) is not type(v) or info[k] != v for k, v in expected.items()):
        raise RuntimeError('Adapter protocol, pins, interlocks or lab build are incompatible')


def lab_exchange(port, mv, trace, running=False, cycle=False, progbuf=False, profile=0):
    request = secrets.token_hex(8)
    name = 'lab-memory-running-spare' if running else 'lab-memory'
    token = 'EXPLICIT_RUNNING_SPARE_MEMORY_LAB' if running else 'EXPLICIT_HALTED_MEMORY_LAB'
    if cycle:
        name, token = 'lab-memory-halt-cycle', 'EXPLICIT_HALT_MEMORY_RESUME'
    if progbuf:
        name, token = 'lab-progbuf-halt-cycle', 'EXPLICIT_PROGBUF_LOAD_RESUME'
    if profile:
        if not progbuf or profile not in range(1, 5):
            raise ValueError('Invalid fixed word profile')
        name = 'lab-progbuf-word'
    command = f'{name} {request} {mv} PINS25_26_WIRING_CHECKED {token}'
    if profile:
        command += f' {profile}'
    # One request, no host retry/reset/fallback; cycle mode owns bounded halt/resume.
    return diag.exchange(port, command, 'lab-memory', request=request, timeout=45, trace=trace)


def inspect_samples(result, reference, prefix_reference, running=False, original=False, cycle=False, progbuf=False):
    """Byte checks are evidence of sample consistency, never original-device proof."""
    raw = []
    failures = []
    if any(result.get(k) != 'ok' for k in ('result', 'operation_result', 'cleanup_result')):
        failures.append('operation_or_cleanup_failed')
    for key in ('data_restored', 'final_valid', 'pins_released'):
        if result.get(key) is not True:
            failures.append(key + '_unconfirmed')
    if result.get('target_state_uncertain') is not False or result.get('trace_overflow') is not False:
        failures.append('state_or_trace_unconfirmed')
    if progbuf and any(result.get(k) is not True for k in ('progbuf', 'gprs_restored', 'program_restored', 'pc_unchanged')):
        failures.append('progbuf_restoration_unconfirmed')
    expected_status = '00000c82' if running else '00000382'
    actual_status = result.get('final_dmstatus')
    if cycle:
        if any(result.get(k) is not True for k in ('halt_cycle', 'halt_observed', 'resume_observed', 'control_restored')):
            failures.append('halt_resume_unconfirmed')
        # Resume-ack latches are expected only after the verified owned cycle.
        actual_status = f"{int(actual_status or '0', 16) & ~0x30000:08x}"
    if result.get('final_dmcontrol') != '00000001' or actual_status != expected_status:
        # Conservatively reject reset/resume latches until reviewed too.
        failures.append('unexpected_final_state')
    if result.get('cmderr') != 0 or result.get('command_attempts') != 16:
        failures.append('command_completion_unconfirmed')
    passes = result.get('passes', [])
    if len(passes) != 2:
        raise ValueError('Expected exactly two passes')
    for words in passes:
        if len(words) > 8:
            raise ValueError('Oversized sample')
        data = bytearray()
        for i, word in enumerate(words):
            if int(word['address'], 16) != ADDRESSES[i]:
                raise ValueError('Unexpected address or word order')
            value = int(word['value'], 16)
            if not 0 <= value <= 0xffffffff:
                raise ValueError('Word is outside uint32 range')
            data.extend(value.to_bytes(4, 'little'))
        raw.append(bytes(data))
    if any(len(data) != 32 for data in raw):
        failures.append('incomplete_samples')
    if raw[0] != raw[1]:
        failures.append('repeat_mismatch')
    if len(reference) < 16 or (not original and len(prefix_reference) != 16):
        raise ValueError('Need application reference >=16 bytes and exactly 16 known spare prefix bytes')
    if not raw[0] or raw[0][:16] != reference[:16]:
        failures.append('application_reference_mismatch')
    if not original and raw[0][16:] != prefix_reference:
        failures.append('prefix_canary_mismatch')
    # Constant/repeating words are not accepted as positive controls. A real
    # original prefix could be repetitive: this rejects a lab control, not data.
    for offset in (0, 16):
        window = raw[0][offset:offset+16]
        if len(set(window[i:i+4] for i in range(0, len(window), 4))) < 2:
            failures.append('constant_or_stale_window')
    if raw[0][:16] == raw[0][16:]:
        failures.append('possible_alias_or_stale_window')
    return raw, {'sample_checks_passed': not failures, 'failures': failures,
                 'recovery_verified': False,
                 'source': 'user_authorized_original_DMT99' if original else 'operator_attested_unprotected_spare',
                 'prefix_reference_available': not original,
                 'repeat_equal': len(raw[0]) == 32 and raw[0] == raw[1],
                 'scope': 'two_16_byte_windows_only_not_full_flash_or_prefix'}


def inspect_word_samples(result, name):
    profile, address = WORD_PROFILES[name]
    failures, raw = [], []
    for k in ('result', 'operation_result', 'cleanup_result'):
        if result.get(k) != 'ok': failures.append(k + '_failed')
    for k in ('data_restored', 'final_valid', 'pins_released', 'halt_cycle', 'halt_observed',
              'resume_observed', 'control_restored', 'progbuf', 'gprs_restored', 'program_restored', 'pc_unchanged'):
        if result.get(k) is not True: failures.append(k + '_unconfirmed')
    if result.get('target_state_uncertain') is not False or result.get('trace_overflow') is not False:
        failures.append('state_or_trace_unconfirmed')
    if result.get('final_dmcontrol') != '00000001' or (int(result.get('final_dmstatus', '0'), 16) & ~0x30000) != 0xc82:
        failures.append('unexpected_final_state')
    if result.get('profile') != profile or result.get('command_attempts') != 2 or result.get('cmderr') != 0:
        failures.append('command_completion_unconfirmed')
    passes = result.get('passes', [])
    if len(passes) != 2: raise ValueError('Expected two word passes')
    for words in passes:
        if len(words) > 1: raise ValueError('Oversized word capture')
        if not words:
            raw.append(b''); failures.append('incomplete_samples'); continue
        word = words[0]
        if int(word['address'], 16) != address: raise ValueError('Wrong word address')
        value = int(word['value'], 16)
        if not 0 <= value <= 0xffffffff: raise ValueError('Invalid word')
        raw.append(value.to_bytes(4, 'little'))
    repeat = len(raw[0]) == 4 and raw[0] == raw[1]
    if not repeat: failures.append('repeat_unconfirmed')
    return raw, {'sample_checks_passed': not failures, 'failures': failures, 'repeat_equal': repeat,
                 'scope': 'one_fixed_word_twice', 'profile': name, 'address': f'{address:08x}',
                 'recovery_verified': False, 'known_reference_available': False,
                 'mapped_source': 'unknown_boot_mapping' if 'alias' in name else name}


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('mode', choices=['audit', 'connect', 'spare-lab', 'original-lab', 'original-halt-lab', 'original-progbuf-lab', 'original-progbuf-word'])
    p.add_argument('--captures', type=Path, default=ROOT / 'captures')
    p.add_argument('--reference', type=Path, default=ROOT.parents[1] / 'DMT-99_V2.4_20240306.bin')
    p.add_argument('--output', type=Path, required=True, help='New session directory; never overwritten')
    p.add_argument('--word-profile', choices=list(WORD_PROFILES))
    p.add_argument('--port', default='/dev/ttyUSB0')
    p.add_argument('--voltage-mv', type=int)
    p.add_argument('--allow-original-progbuf-load', action='store_true')
    p.add_argument('--allow-original-halt-read-resume', action='store_true')
    p.add_argument('--unprotected-spare-confirmed', action='store_true')
    p.add_argument('--allow-original-running-read', action='store_true',
                   help='Separate authorization: original target, fixed running-hart read experiment only')
    p.add_argument('--hart-state', choices=['halted', 'running'], help='Existing spare hart state; never changed')
    p.add_argument('--board-pins-and-wiring-checked', action='store_true')
    p.add_argument('--prefix-reference', type=Path, help='Exactly 16 known canary bytes on the spare')
    args = p.parse_args(argv)
    word_mode = args.mode == 'original-progbuf-word'
    if word_mode != bool(args.word_profile):
        p.error('word-profile is required only for original-progbuf-word')
    profile = WORD_PROFILES[args.word_profile][0] if word_mode else 0
    progbuf = word_mode or args.mode == 'original-progbuf-lab'
    cycle = progbuf or args.mode == 'original-halt-lab'
    original = args.mode in ('original-lab', 'original-halt-lab', 'original-progbuf-lab', 'original-progbuf-word')
    authorized = (args.allow_original_progbuf_load and args.allow_original_halt_read_resume) if progbuf else args.allow_original_halt_read_resume if cycle else args.allow_original_running_read
    if original and not (authorized and args.board_pins_and_wiring_checked
            and args.voltage_mv is not None and 3000 <= args.voltage_mv <= 3450
            and args.hart_state in (None, 'running') and not args.unprotected_spare_confirmed):
        p.error('Original experiment requires its matching explicit authorization, wiring and fresh voltage')
    if args.mode == 'spare-lab' and not (
        args.unprotected_spare_confirmed and args.hart_state and args.board_pins_and_wiring_checked and
        args.voltage_mv is not None and 3000 <= args.voltage_mv <= 3450 and args.prefix_reference):
        p.error('spare-lab requires an unprotected spare, existing hart state, wiring, fresh voltage and prefix reference')
    if args.output.exists():
        p.error('Output exists; use a new directory')
    if not args.captures.is_dir() or not args.reference.is_file():
        p.error('Missing evidence directory or known reference BIN')
    # Validate reference inputs before opening any device or creating output.
    prefix = args.prefix_reference.read_bytes() if args.mode == 'spare-lab' else None
    reference = args.reference.read_bytes()
    if original and digest(reference) != '4befdf6b58e063f03901f190000d790e60d20d0a23e5eeb2b6f6840f1830e98e':
        p.error('Original experiment requires the verified V2.4 reference BIN')
    if args.mode == 'spare-lab' and (len(prefix) != 16 or len(reference) < 16):
        p.error('Invalid lab reference lengths')
    report = audit(args.captures, args.reference)
    report.update(utc=dt.datetime.now(dt.timezone.utc).isoformat(), mode=args.mode,
                  port=args.port, target_operations_executed=False, recovered_bytes=0)
    args.output.mkdir(parents=True, exist_ok=False)
    port = None
    uart_log = None
    try:
        if args.mode != 'audit':
            import serial
            port = serial.Serial(port=None, baudrate=115200, timeout=.25,
                                 write_timeout=2, exclusive=True)
            port.dtr = False
            port.rts = False
            port.port = args.port
            uart_log = (args.output / 'uart.raw.log').open('xb')
            original_readline = port.readline
            def logged_readline():
                data = original_readline()
                if data:
                    uart_log.write(data)
                    uart_log.flush()
                return data
            port.readline = logged_readline
            port.open()
            port.reset_input_buffer()
            info = diag.synchronize_info(port)
            report['adapter'] = info
            running = original or args.hart_state == 'running'
            validate_adapter(info, args.mode in ('spare-lab', 'original-lab', 'original-halt-lab', 'original-progbuf-lab', 'original-progbuf-word'), running, cycle, progbuf)
            if args.mode in ('spare-lab', 'original-lab', 'original-halt-lab', 'original-progbuf-lab', 'original-progbuf-word'):
                report['operator_voltage_mv'] = args.voltage_mv
                report['requested_hart_state'] = 'running' if running else 'halted'
                report['target_identity_attestation'] = 'original_DMT99' if original else 'unprotected_spare'
                if original:
                    report['authorization'] = 'explicit_original_progbuf_load_halt_resume' if progbuf else 'explicit_original_halt_read_resume' if cycle else 'explicit_original_running_read_only'
                    # Retain the already flashed/tested wire ABI. Its legacy
                    # name selects the running path; it does not identify a chip
                    # as unprotected. Never falsify a spare attestation.
                    report['wire_abi_note'] = 'explicit owned halt/read/resume cycle' if cycle else 'lab-memory-running-spare reused with explicit original-target authorization'
                report['target_operations_executed'] = True
                report['dmi_trace'] = []
                result = lab_exchange(port, args.voltage_mv, report['dmi_trace'], running, cycle, progbuf, profile)
                report['lab_result'] = result
                raw, checks = inspect_word_samples(result, args.word_profile) if word_mode else inspect_samples(result, reference, prefix, running, original, cycle, progbuf)
                report['sample_validation'] = checks
                if not original:
                    report['prefix_reference'] = file_evidence(args.prefix_reference)
                report['artifacts'] = []
                regions = [(args.word_profile, 0, WORD_PROFILES[args.word_profile][1])] if word_mode else [('application', 0, 0x08006400), ('prefix', 16, 0x08000000)]
                for n, data in enumerate(raw, 1):
                    for label, offset, address in regions:
                        payload = data[offset:offset+16]
                        if not payload:
                            continue
                        source = 'original' if original else 'spare'
                        path = args.output / f'{source}-{label}-pass{n}.unverified.bin'
                        with path.open('xb') as f:
                            f.write(payload)
                        report['artifacts'].append({**file_evidence(path), 'address': f'{address:08x}',
                                                    'pass': n, 'original_dmt99': original})
                if not checks['sample_checks_passed']:
                    raise RuntimeError('Sample checks failed; no retry or larger read')
                report['decision'] = ('bounded_original_samples_checked_no_automatic_expansion'
                                      if original else 'spare_samples_checked_original_target_still_requires_review')
            else:
                report['decision'] = 'no_validated_original_target_read_path_no_scan_repeated'
    except (OSError, RuntimeError, TimeoutError, ValueError, TypeError, KeyError, AttributeError, ImportError) as exc:
        report['error'] = str(exc)
        report['error_type'] = type(exc).__name__
        report['decision'] = 'stop_no_retry'
    except KeyboardInterrupt:
        report['error'] = 'operator_interrupted_no_retry_target_completion_unconfirmed'
    finally:
        if port is not None:
            try:
                port.close()
            except OSError as exc:
                report['error'] = 'Serial close failed: ' + str(exc)
        if uart_log is not None:
            uart_log.close()
            report['raw_uart'] = file_evidence(args.output / 'uart.raw.log')
        with (args.output / 'session.json').open('x') as f:
            json.dump(report, f, indent=2)
            f.write('\n')
    print(json.dumps({'report': str(args.output / 'session.json'), 'decision': report['decision'],
                      'recovered_bytes': 0, 'error': report.get('error')}, indent=2))
    return 1 if 'error' in report else 0


if __name__ == '__main__':
    sys.exit(main())
