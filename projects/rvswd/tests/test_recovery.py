import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from types import SimpleNamespace
from unittest.mock import patch
from test_host import ROOT, module

recovery = module('recovery')


def result_fixture():
    # Synthetic bytes, deliberately distinct by window and word.
    values = list(range(0x11223340, 0x11223344)) + list(range(0x99887760, 0x99887764))
    words = [{'address': f'{address:08x}', 'value': f'{value:08x}'}
             for address, value in zip(recovery.ADDRESSES, values)]
    raw = b''.join(value.to_bytes(4, 'little') for value in values)
    return {'result': 'ok', 'operation_result': 'ok', 'cleanup_result': 'ok',
            'data_restored': True, 'final_valid': True, 'pins_released': True,
            'target_state_uncertain': False, 'trace_overflow': False,
            'final_dmcontrol': '00000001', 'final_dmstatus': '00000382',
            'cmderr': 0, 'command_attempts': 16, 'passes': [words, [dict(w) for w in words]]}, raw


class RecoveryTests(unittest.TestCase):
    def test_controller_serial_session_and_artifacts(self):
        info = {'event': 'info', 'protocol': 3, 'build': 'read-only-diagnostic',
                'adapter_target': 'esp32', 'console': 'uart0', 'gpio_clk': 25, 'gpio_dio': 26,
                'initialized': True, 'board_verified': True, 'target_memory_write': False,
                'flash_write': False, 'reset_command': False, 'identify_writes': False,
                'halt_resume_enabled': False, 'abstract_memory_lab': True,
                'memory_lab_requires_halted': True, 'abstract_memory_running_validated': False,
                'running_spare_lab': True}
        result, raw = result_fixture()
        class SerialPeer:
            def __init__(self, **kwargs):
                self.commands = []; self.queue = []; self.closed = False
                self.dtr = self.rts = True
            def open(self):
                assert self.dtr is False and self.rts is False
            def close(self): self.closed = True
            def flush(self): pass
            def reset_input_buffer(self): pass
            def write(self, data):
                command = data.decode().strip(); self.commands.append(command)
                if command == 'hello': response = info
                elif command.startswith(('lab-memory ', 'lab-memory-running-spare ')):
                    response = {**result, 'event': 'lab-memory', 'request': command.split()[1]}
                    if command.startswith('lab-memory-running-spare '):
                        response['final_dmstatus'] = '00000c82'
                else: raise AssertionError(command)
                self.queue.append((json.dumps(response)+'\n').encode())
            def readline(self): return self.queue.pop(0) if self.queue else b''
        with tempfile.TemporaryDirectory() as tmp:
            base = Path(tmp); captures = base / 'captures'; captures.mkdir()
            reference = base / 'app.bin'; reference.write_bytes(raw[:16])
            prefix = base / 'prefix.bin'; prefix.write_bytes(raw[16:])
            common = ['--captures', str(captures), '--reference', str(reference)]
            for mode, state in [('connect', None), ('spare-lab', 'halted'), ('spare-lab', 'running')]:
                peer = SerialPeer()
                out = base / (mode + '-' + str(state))
                args = [mode, '--output', str(out), *common]
                if mode == 'spare-lab':
                    args += ['--unprotected-spare-confirmed', '--hart-state', state, '--board-pins-and-wiring-checked',
                             '--voltage-mv', '3300', '--prefix-reference', str(prefix)]
                with patch.dict(sys.modules, {'serial': SimpleNamespace(Serial=lambda **kw: peer)}):
                    self.assertEqual(recovery.main(args), 0)
                self.assertTrue(peer.closed)
                self.assertEqual(len(peer.commands), 1 if mode == 'connect' else 2)
                record = json.loads((out / 'session.json').read_text())
                self.assertFalse(record['recovery_verified'])
                if mode == 'connect': self.assertFalse(record['target_operations_executed'])
                else:
                    self.assertEqual(len(record['artifacts']), 4)
                    for artifact in record['artifacts']:
                        payload = Path(artifact['path']).read_bytes()
                        self.assertEqual(payload, raw[:16] if artifact['address'] == '08006400' else raw[16:])
                        self.assertEqual(artifact['sha256'], recovery.digest(payload))
                        self.assertFalse(artifact['original_dmt99'])

    def test_independent_byte_checks_do_not_claim_original_recovery(self):
        result, raw = result_fixture()
        passes, checks = recovery.inspect_samples(result, raw[:16], raw[16:])
        self.assertEqual(passes, [raw, raw])
        self.assertTrue(checks['sample_checks_passed'])
        self.assertFalse(checks['recovery_verified'])

    def test_original_samples_do_not_invent_a_prefix_reference(self):
        result, raw = result_fixture()
        result['final_dmstatus'] = '00000c82'
        passes, checks = recovery.inspect_samples(result, raw[:16], None, True, True)
        self.assertTrue(checks['sample_checks_passed'])
        self.assertEqual(passes, [raw, raw])
        self.assertEqual(checks['source'], 'user_authorized_original_DMT99')
        self.assertFalse(checks['prefix_reference_available'])
        self.assertFalse(checks['recovery_verified'])
        result['passes'][1][0]['value'] = '00000000'
        self.assertFalse(recovery.inspect_samples(result, raw[:16], None, True, True)[1]['sample_checks_passed'])

    def test_halt_cycle_wire_command_is_explicit_and_sent_once(self):
        with patch.object(recovery.diag, 'exchange', return_value={'result': 'mock'}) as exchange:
            port, trace = object(), []
            self.assertEqual(recovery.lab_exchange(port, 3000, trace, True, True), {'result': 'mock'})
            exchange.assert_called_once()
            command = exchange.call_args.args[1].split()
            self.assertEqual(command[0], 'lab-memory-halt-cycle')
            self.assertEqual(command[2:], ['3000', 'PINS25_26_WIRING_CHECKED', 'EXPLICIT_HALT_MEMORY_RESUME'])
            self.assertEqual(exchange.call_args.kwargs['timeout'], 45)

    def test_progbuf_checks_require_gpr_program_and_pc_restoration(self):
        result, raw = result_fixture()
        result.update(final_dmstatus='00000c82', halt_cycle=True, halt_observed=True,
                      resume_observed=True, control_restored=True, progbuf=True,
                      gprs_restored=True, program_restored=True, pc_unchanged=True)
        self.assertTrue(recovery.inspect_samples(result, raw[:16], None, True, True, True, True)[1]['sample_checks_passed'])
        for key in ('gprs_restored', 'program_restored', 'pc_unchanged'):
            self.assertFalse(recovery.inspect_samples({**result, key: False}, raw[:16], None, True, True, True, True)[1]['sample_checks_passed'])
        with patch.object(recovery.diag, 'exchange', return_value={}) as exchange:
            recovery.lab_exchange(object(), 3000, [], True, True, True)
            self.assertEqual(exchange.call_args.args[1].split()[0], 'lab-progbuf-halt-cycle')
            self.assertEqual(exchange.call_args.args[1].split()[-1], 'EXPLICIT_PROGBUF_LOAD_RESUME')

    def test_fixed_words_address_repeat_and_scope(self):
        for name, (profile, address) in recovery.WORD_PROFILES.items():
            result, _ = result_fixture()
            result.update(final_dmstatus='00000c82', halt_cycle=True, halt_observed=True,
                          resume_observed=True, control_restored=True, progbuf=True,
                          gprs_restored=True, program_restored=True, pc_unchanged=True,
                          profile=profile, command_attempts=2,
                          passes=[[{'address': f'{address:08x}', 'value': '12345678'}] for _ in range(2)])
            raw, checks = recovery.inspect_word_samples(result, name)
            self.assertEqual(raw, [bytes.fromhex('78563412')]*2)
            self.assertTrue(checks['sample_checks_passed'])
            self.assertFalse(checks['recovery_verified'])
            result['passes'][1][0]['value'] = '12345679'
            self.assertFalse(recovery.inspect_word_samples(result, name)[1]['sample_checks_passed'])
            result['passes'][1][0]['address'] = 'deadbeef'
            with self.assertRaises(ValueError): recovery.inspect_word_samples(result, name)
            with patch.object(recovery.diag, 'exchange', return_value={}) as exchange:
                recovery.lab_exchange(object(), 3000, [], True, True, True, profile)
                command = exchange.call_args.args[1].split()
                self.assertEqual(command[0], 'lab-progbuf-word')
                self.assertEqual(command[-1], str(profile))

    def test_halt_cycle_requires_resume_and_accepts_only_resume_ack_latches(self):
        result, raw = result_fixture()
        result.update(final_dmstatus='00030c82', halt_cycle=True, halt_observed=True,
                      resume_observed=True, control_restored=True)
        self.assertTrue(recovery.inspect_samples(result, raw[:16], None, True, True, True)[1]['sample_checks_passed'])
        result['resume_observed'] = False
        self.assertFalse(recovery.inspect_samples(result, raw[:16], None, True, True, True)[1]['sample_checks_passed'])
        result['resume_observed'] = True
        result['final_dmstatus'] = '000f0c82'
        self.assertFalse(recovery.inspect_samples(result, raw[:16], None, True, True, True)[1]['sample_checks_passed'])

    def test_halt_cycle_needs_separate_cli_authorization(self):
        with tempfile.TemporaryDirectory() as tmp:
            proc = subprocess.run([sys.executable, str(ROOT / 'tools/recovery.py'),
                'original-halt-lab', '--output', str(Path(tmp) / 'capture'),
                '--voltage-mv', '3000', '--board-pins-and-wiring-checked',
                '--allow-original-running-read'], capture_output=True)
            self.assertEqual(proc.returncode, 2)
            self.assertFalse((Path(tmp) / 'capture').exists())

    def test_original_requires_its_own_explicit_gate(self):
        with tempfile.TemporaryDirectory() as tmp:
            for extra in ([], ['--unprotected-spare-confirmed']):
                proc = subprocess.run([sys.executable, str(ROOT / 'tools/recovery.py'),
                    'original-lab', '--output', str(Path(tmp) / 'capture'),
                    '--voltage-mv', '3000', '--board-pins-and-wiring-checked', *extra], capture_output=True)
                self.assertEqual(proc.returncode, 2)
                self.assertFalse((Path(tmp) / 'capture').exists())

    def test_success_status_cannot_hide_bad_bytes(self):
        for failure in ('repeat', 'reference', 'prefix', 'constant', 'alias', 'short', 'cleanup', 'busy', 'trace'):
            with self.subTest(failure=failure):
                result, raw = result_fixture()
                reference, prefix = raw[:16], raw[16:]
                if failure == 'repeat': result['passes'][1][0]['value'] = '11223345'
                if failure == 'reference': reference = bytes(16)
                if failure == 'prefix': prefix = bytes(16)
                if failure == 'constant':
                    for p in result['passes']:
                        for w in p: w['value'] = 'ffffffff'
                if failure == 'alias':
                    for p in result['passes']:
                        for i in range(4): p[i+4]['value'] = p[i]['value']
                if failure == 'short': result['passes'][1].pop()
                if failure == 'cleanup': result['data_restored'] = False
                if failure == 'busy': result['cmderr'] = 1
                if failure == 'trace': result['trace_overflow'] = True
                _, checks = recovery.inspect_samples(result, reference, prefix)
                self.assertFalse(checks['sample_checks_passed'])
                self.assertFalse(checks['recovery_verified'])

    def test_address_and_word_bounds(self):
        for bad in ('address', 'negative', 'overflow', 'passes'):
            result, raw = result_fixture()
            if bad == 'address': result['passes'][0][0]['address'] = '40022000'
            if bad == 'negative': result['passes'][0][0]['value'] = '-1'
            if bad == 'overflow': result['passes'][0][0]['value'] = '100000000'
            if bad == 'passes': result['passes'].append([])
            with self.assertRaises((ValueError, OverflowError)):
                recovery.inspect_samples(result, raw[:16], raw[16:])

    def test_no_lab_without_all_interlocks(self):
        with tempfile.TemporaryDirectory() as tmp:
            proc = subprocess.run([sys.executable, str(ROOT / 'tools/recovery.py'),
                'spare-lab', '--output', str(Path(tmp) / 'capture'), '--port', '/not/a/device'],
                capture_output=True)
            self.assertEqual(proc.returncode, 2)
            self.assertFalse((Path(tmp) / 'capture').exists())

    def test_adapter_mismatch_fails_closed(self):
        for info in ({}, {'protocol': 2}, {'protocol': True}):
            with self.assertRaises(RuntimeError): recovery.validate_adapter(info, True)

    def test_exchange_is_single_bounded_command(self):
        result, _ = result_fixture()
        def reply(port, command, event, **kwargs):
            self.assertEqual(event, 'lab-memory')
            self.assertEqual(command.split()[2:], ['3300', 'PINS25_26_WIRING_CHECKED', 'EXPLICIT_HALTED_MEMORY_LAB'])
            self.assertEqual(kwargs['timeout'], 45)
            return result
        with patch.object(recovery.diag, 'exchange', side_effect=reply) as exchange:
            self.assertIs(recovery.lab_exchange(None, 3300, []), result)
            self.assertEqual(exchange.call_count, 1)
        with patch.object(recovery.diag, 'exchange', side_effect=TimeoutError) as exchange:
            with self.assertRaises(TimeoutError): recovery.lab_exchange(None, 3300, [])
            self.assertEqual(exchange.call_count, 1)

    def test_audit_and_missing_device_preserve_evidence(self):
        with tempfile.TemporaryDirectory() as tmp:
            base = Path(tmp)
            captures = base / 'captures'; captures.mkdir()
            evidence = captures / 'old.json'; evidence.write_text('{"command":"capabilities"}')
            reference = base / 'reference.bin'; reference.write_bytes(bytes(range(32)))
            before = evidence.read_bytes()
            common = ['--captures', str(captures), '--reference', str(reference)]
            self.assertEqual(recovery.main(['audit', '--output', str(base / 'audit'), *common]), 0)
            self.assertEqual(recovery.main(['connect', '--output', str(base / 'session'),
                                           '--port', '/not/a/device', *common]), 1)
            record = json.loads((base / 'session/session.json').read_text())
            self.assertFalse(record['target_operations_executed'])
            self.assertEqual(record['recovered_bytes'], 0)
            self.assertIn('error', record)
            self.assertEqual(evidence.read_bytes(), before)
            self.assertFalse(list((base / 'session').glob('*.bin')))


if __name__ == '__main__':
    unittest.main()
