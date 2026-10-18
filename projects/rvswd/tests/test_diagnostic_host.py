import json
import os
from pathlib import Path
import select
import subprocess
import sys
import tempfile
import threading
import unittest
from test_host import ROOT, module


@unittest.skipUnless(__import__('importlib').util.find_spec('serial') and os.name == 'posix',
                     'pyserial and POSIX pseudoterminals required')
class DiagnosticHostTests(unittest.TestCase):
    def capture(self, command, flags=(), info_update=None, result_update=None):
        master, slave = os.openpty()
        commands = []
        stopped = threading.Event()
        errors = []
        info = {'event': 'info', 'protocol': 3, 'build': 'read-only-diagnostic',
                'memory_read': True, 'dmi_write': True, 'target_memory_write': False,
                'flash_write': False, 'flash_dump': False, 'reset_command': False,
                'diagnostic_modes': True, 'identify_writes': False,
                'dmactive_requires_explicit_command': True, 'dm_activation_enabled': False,
                'halt_resume_enabled': False,
                'board_verified': True, 'initialized': True, 'adapter_target': 'esp32',
                'console': 'uart0', 'baud': 115200, 'console_tx': 1, 'console_rx': 3,
                'gpio_clk': 25, 'gpio_dio': 26}
        info.update(info_update or {})

        def peer():
            pending = b''
            try:
                while not stopped.is_set():
                    if not select.select([master], [], [], 0.1)[0]:
                        continue
                    pending += os.read(master, 4096)
                    while b'\n' in pending:
                        line, pending = pending.split(b'\n', 1)
                        text = line.decode().strip()
                        if not text:
                            continue
                        commands.append(text)
                        if text == 'hello':
                            response = info
                        else:
                            response = {'event': command, 'request': text.split()[1],
                                        'result': 'ok', 'operation_result': 'ok',
                                        'cleanup_result': 'ok', 'completed_samples': 8,
                                        'dmstatus': '00000c82', 'dmcontrol': '00000001',
                                        'pins_released': True, 'target_state_uncertain': False,
                                        'snapshots': []}
                            response.update(result_update or {})
                        os.write(master, (json.dumps(response) + '\n').encode())
            except Exception as exc:
                errors.append(exc)

        thread = threading.Thread(target=peer, daemon=True)
        try:
            with tempfile.TemporaryDirectory() as tmp:
                output = Path(tmp) / 'capture.json'
                thread.start()
                proc = subprocess.run([sys.executable, str(ROOT / 'tools/diag.py'),
                                       '--port', os.ttyname(slave), '--output', str(output),
                                       command, '--voltage-mv', '3300', '--board-pins-checked',
                                       '--wiring-checked', *flags], capture_output=True, timeout=10)
                record = json.loads(output.read_text())
                stopped.set()
                thread.join(1)
                self.assertFalse(errors, errors)
                return proc, record, commands
        finally:
            stopped.set()
            thread.join(1)
            os.close(master)
            os.close(slave)

    def test_adapter_restart_is_reported_without_retry(self):
        proc, record, commands = self.capture('identify', result_update={'event': 'info'})
        self.assertEqual(proc.returncode, 1)
        self.assertIn('restarted', record['error'])
        self.assertEqual(len(commands), 2)

    def test_default_has_no_snapshot_or_state_change_selection(self):
        proc, record, commands = self.capture('capabilities')
        self.assertEqual(proc.returncode, 0, proc.stderr)
        self.assertEqual(commands[-1].split()[2:], ['3300', 'PINS25_26_WIRING_CHECKED', '0'])
        self.assertEqual(len(commands), 2)
        self.assertFalse(record['memory_read'])
        self.assertTrue(record['decoded']['dmstatus']['allrunning'])
        self.assertFalse(record['decoded']['dmstatus']['allhalted'])
        self.assertTrue(record['decoded']['dmcontrol']['dmactive'])

    def test_bounded_snapshot_selection_and_capture(self):
        proc, record, commands = self.capture('capabilities', ['--sram', '--gpio', '--usart', '--spi'],
            result_update={'snapshots': [{'name': 'SRAM', 'address': '20000000',
                                         'value': '12345678', 'width_bits': 32}]})
        self.assertEqual(proc.returncode, 0, proc.stderr)
        self.assertEqual(commands[-1].split()[-1], '30')
        self.assertTrue(record['memory_read'])
        self.assertFalse(any('halt' in c or 'activate' in c or 'dump' in c for c in commands))

    def test_no_implicit_halt_for_register_selection(self):
        proc, record, commands = self.capture('capabilities', ['--registers'],
            result_update={'result': 'hart_running_no_implicit_halt'})
        self.assertEqual(proc.returncode, 1)
        self.assertEqual(commands[-1].split()[-1], '1')
        self.assertEqual(len(commands), 2)
        self.assertFalse(record['memory_read'])

    def test_independent_build_gates_prevent_state_changing_wire_commands(self):
        for command, flag in [('halt-cycle', '--allow-halt-resume'),
                              ('activate-dm', '--allow-dm-activation')]:
            with self.subTest(command=command):
                proc, record, commands = self.capture(command, [flag])
                self.assertEqual(proc.returncode, 1)
                self.assertIn('disabled', record['error'])
                self.assertEqual(commands, ['hello'])

    def test_explicit_cycle_has_token_and_no_hidden_selection(self):
        proc, _, commands = self.capture('halt-cycle', ['--allow-halt-resume'],
                                        info_update={'halt_resume_enabled': True})
        self.assertEqual(proc.returncode, 0, proc.stderr)
        self.assertEqual(commands[-1].split()[2:],
                         ['3300', 'PINS25_26_WIRING_CHECKED', 'EXPLICIT_HALT_RESUME', '0'])
        self.assertEqual(len(commands), 2)

    def test_activation_gate_is_independent_and_wire_token_is_explicit(self):
        proc, _, commands = self.capture('activate-dm', ['--allow-dm-activation'],
            info_update={'dm_activation_enabled': True, 'halt_resume_enabled': False},
            result_update={'dmactive_write_attempted': True, 'dmactive_set': True,
                           'dmcontrol_readback_valid': True, 'dmstatus_readback_valid': True,
                           'activation_state_uncertain': False})
        self.assertEqual(proc.returncode, 0, proc.stderr)
        self.assertEqual(commands[-1].split()[2:],
                         ['3300', 'PINS25_26_WIRING_CHECKED', 'EXPLICIT_DM_ACTIVATION'])
        self.assertEqual(len(commands), 2)

    def test_old_or_wrong_pin_firmware_cannot_receive_probe(self):
        for update in [{'build': 'flash-reader', 'protocol': 2}, {'gpio_dio': 17}, {'flash_dump': True}]:
            with self.subTest(update=update):
                proc, record, commands = self.capture('capabilities', info_update=update)
                self.assertEqual(proc.returncode, 1)
                self.assertIn('error', record)
                self.assertEqual(commands, ['hello'])

    def test_cleanup_evidence_is_preserved_and_never_retried(self):
        for update in [{'pins_released': False}, {'target_state_uncertain': True}, {'trace_overflow': True}]:
            with self.subTest(update=update):
                proc, record, commands = self.capture('capabilities', result_update=update)
                self.assertEqual(proc.returncode, 1)
                self.assertIn('error', record)
                self.assertEqual(len(commands), 2)
                for key, value in update.items():
                    self.assertEqual(record['diagnostic'][key], value)


class DiagnosticArgumentTests(unittest.TestCase):
    def test_forbidden_and_unconfirmed_commands_fail_before_serial_open(self):
        for command in ['dump', 'reset', 'halt-cycle', 'activate-dm']:
            with self.subTest(command=command), tempfile.TemporaryDirectory() as tmp:
                proc = subprocess.run([sys.executable, str(ROOT / 'tools/diag.py'),
                                       '--port', '/not/a/device', '--output', str(Path(tmp) / 'x.json'),
                                       command, '--voltage-mv', '3300', '--board-pins-checked',
                                       '--wiring-checked'], capture_output=True)
                self.assertEqual(proc.returncode, 2)
                self.assertNotIn(b'No such file or directory', proc.stderr)

    def test_status_decode_uses_correct_hart_selection(self):
        d = module('diag').decode_status({'dmstatus': '00030382',
              'dmcontrol': f'{(3 << 16) | (2 << 6) | 1:08x}', 'completed_samples': 8})
        self.assertEqual(d['dmcontrol']['hartsel'], 3 + 2 * 1024)
        self.assertTrue(d['dmstatus']['allhalted'])
        self.assertTrue(d['dmstatus']['allresumeack'])
