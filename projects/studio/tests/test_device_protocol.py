"""USB management protocol against the simulated device (firmware UI code + flash copy)."""
import hashlib
import io
import struct
import threading
import time

import pytest
from PIL import Image

from dmt99studio import paths
from dmt99studio.core import backup, jpegtools
from dmt99studio.core.device import DeviceClient
from dmt99studio.core.flashimage import FlashImage, SECTOR
from dmt99studio.core.protocol import Cmd, Decoder, ProtocolError, Status, crc32, encode_request, encode_response

RELEASE_BIN = paths.FINAL_RELEASES / 'v0.3.1' / 'DMT-99_V0.3.1_20261018.bin'


@pytest.fixture()
def dev(original_path):
    d = DeviceClient.open_simulated(original_path, RELEASE_BIN if RELEASE_BIN.exists() else None)
    d.device_info()
    yield d
    d.close()


def test_codec_resync_and_crc():
    good = encode_response(Cmd.PING, 7, 0, b'abc')
    dec = Decoder()
    frames = dec.feed(b'\x00\xd9garbage' + good[:5])
    assert frames == []
    frames = dec.feed(good[5:] + bytes([good[0], good[1]]) + b'\x07' * 20 + good)  # bad version byte
    assert [f.payload for f in frames] == [b'abc', b'abc']
    bad = bytearray(good)
    bad[-1] ^= 1
    assert Decoder().feed(bytes(bad)) == []
    assert encode_request(Cmd.PING, 1, b'')[-4:] == struct.pack('<I', crc32(encode_request(Cmd.PING, 1, b'')[:-4]))


def test_identity(dev):
    assert dev.ping(b'hello') == b'hello'
    i = dev.info
    assert 'simulated' in i['product'] and i['protocol'] == 1 and i['spi_capacity'] == 8 * 1024 * 1024
    assert {'spi_read', 'spi_write_assets', 'measurement', 'settings_get'} <= set(i['capability_names'])
    names = {r['name']: r for r in dev.memory_map()}
    assert names['assets-fat12']['writable'] and not names['boot-fat12']['writable']
    assert names['assets-fat12']['start'] == 463 * SECTOR
    assert dev.spi_id()['jedec'] == 0xEF4017
    assert dev.capabilities()['max_read'] == 4096


def test_reads_match_the_image(dev, original_path):
    local = original_path.read_bytes()
    assert dev.read_spi(0, 9000) == local[:9000]
    assert dev.read_partition(2, 100, 5000) == local[463 * SECTOR + 100:463 * SECTOR + 5100]
    assert dev.spi_crc(463 * SECTOR, 200000) == crc32(local[463 * SECTOR:463 * SECTOR + 200000])
    assert dev.compare_sectors(local, 460, 10) == []


def test_bad_frames_and_ranges(dev):
    raw = bytearray(encode_request(Cmd.PING, 99, b'x'))
    raw[-1] ^= 0xFF
    dev.t.write(bytes(raw))
    time.sleep(0.2)
    assert dev.ping(b'still alive') == b'still alive'
    with pytest.raises(ProtocolError) as e:
        dev.request(Cmd.READ_SPI_FLASH, struct.pack('<IH', 8 * 1024 * 1024 - 10, 100))
    assert e.value.status == Status.RANGE
    with pytest.raises(ProtocolError) as e:
        dev.request(Cmd.READ_SPI_FLASH, struct.pack('<IH', 0, 5000))
    assert e.value.status == Status.RANGE
    with pytest.raises(ProtocolError) as e:
        dev.request(0x7F)
    assert e.value.status == Status.COMMAND


def test_write_rules(dev):
    token = dev._begin()
    try:
        for sector in (0, 62, 63, 462, 2048):  # MBR, BOOT volume, beyond the chip
            with pytest.raises(ProtocolError) as e:
                dev._write_op(2, token, struct.pack('<H', sector))
            assert e.value.status == Status.DENIED
        with pytest.raises(ProtocolError) as e:   # program without erase
            dev._write_op(3, token, struct.pack('<I', 500 * SECTOR) + b'\0' * 16)
        assert e.value.status == Status.DENIED
        with pytest.raises(ProtocolError) as e:   # wrong token
            dev._write_op(2, token ^ 1, struct.pack('<H', 500))
        assert e.value.status == Status.SESSION
        with pytest.raises(ProtocolError) as e:   # second session
            dev.request(Cmd.WRITE_SPI_ASSET_REGION, b'\x01ASET')
        assert e.value.status == Status.BUSY
        original = dev.read_spi(600 * SECTOR, SECTOR)
        dev._write_op(2, token, struct.pack('<H', 600))
        with pytest.raises(ProtocolError) as e:   # page crossing
            dev._write_op(3, token, struct.pack('<I', 600 * SECTOR + 200) + b'\0' * 100)
        assert e.value.status == Status.DENIED
        with pytest.raises(ProtocolError) as e:   # other sector than the erased one
            dev._write_op(3, token, struct.pack('<I', 601 * SECTOR) + b'\0' * 16)
        assert e.value.status == Status.DENIED
    finally:
        dev._write_op(4, token, b'')
    with pytest.raises(ProtocolError) as e:       # session closed
        dev._write_op(2, token, struct.pack('<H', 600))
    assert e.value.status == Status.SESSION
    # Sector 600 was erased in the session above; restore it through the normal path.
    dev.write_sectors({600: original})
    assert dev.read_spi(600 * SECTOR, SECTOR) == original


def test_write_sectors_and_original_untouched(dev, original_path):
    before = hashlib.sha256(original_path.read_bytes()).hexdigest()
    data = bytes((i * 7) & 0xFF for i in range(SECTOR))
    assert dev.write_sectors({1000: data}) == [1000]
    assert dev.read_spi(1000 * SECTOR, SECTOR) == data
    with pytest.raises(ValueError):
        dev.write_sectors({10: data})
    assert hashlib.sha256(original_path.read_bytes()).hexdigest() == before


def test_deploy_assets_end_to_end(dev, tmp_path, original_path):
    image = FlashImage.load(original_path)
    key = '1:/mode/dc_v.jpg'
    info = jpegtools.inspect(image.read_file(image.resolve(key)))
    new = jpegtools.encode(Image.new('RGB', (info.width, info.height), (200, 30, 30)))
    result = backup.deploy_assets(dev, {key: new}, backup_folder=tmp_path)
    assert result['written'] and all(463 <= s < 2048 for s in result['written'])
    assert (tmp_path / result['backup'].split('/')[-1]).exists()
    # Read the device back and parse it like the meter would.
    live = bytearray(b'\xff' * 8 * 1024 * 1024)
    live[:SECTOR] = dev.read_spi(0, SECTOR)
    live[463 * SECTOR:] = dev.read_partition(2, 0, 1585 * SECTOR)
    img = FlashImage(bytes(live))
    assert img.read_file(img.resolve(key)) == new
    assert img.assets.validate() == []
    # Uploading the same content again writes nothing.
    again = backup.deploy_assets(dev, {key: new}, backup_folder=tmp_path)
    assert again['written'] == [] and again['skipped'] == [key]
    # Restore the backup taken before the first upload.
    restored = backup.restore_assets(dev, tmp_path / result['backup'].split('/')[-1])
    assert restored['written']
    assert dev.read_partition(2, 0, 1585 * SECTOR) == original_path.read_bytes()[463 * SECTOR:]


def test_measurement_and_stream(dev):
    assert dev.measurement()['available'] is False      # no packet yet: nothing invented
    events = []
    got = threading.Event()

    def on_event(m):
        events.append(m)
        if m.get('unit_name') == 'V':
            got.set()
    dev.event_callback = on_event
    dev.start_stream(0)
    dev.sim.command('ack 0x0c')
    dev.sim.command('reading 12.34 6')
    assert got.wait(3)
    m = dev.measurement()
    assert m['available'] and m['mode'] == 0x0C and m['unit_name'] == 'V' and abs(m['value'] - 12.34) < 1e-4
    dev.sim.command('reading 0.123 7')
    time.sleep(0.3)
    dev.stop_stream()
    assert any(e.get('unit_name') == 'mV' for e in events)
    dev.sim.command('charge 1')
    time.sleep(0.2)
    m = dev.measurement()
    assert m['available'] is False


def test_settings(dev):
    s = dev.get_settings()
    assert len(s) == 256
    dev.set_setting(5, 2)
    word = struct.unpack_from('<H', dev.get_settings(), 30)[0]
    assert (word >> 8) & 3 == 2
    with pytest.raises(ProtocolError) as e:
        dev.set_setting(5, 9)
    assert e.value.status == Status.ARGUMENT
    with pytest.raises(ProtocolError):
        dev.set_setting(99, 0)


def test_internal_reads_never_fabricate(dev):
    if not RELEASE_BIN.exists():
        pytest.skip('release BIN missing')
    data = dev.read_internal(0x08006400, 8192)
    assert data == RELEASE_BIN.read_bytes()[:8192]
    for region in (0, 1, 2, 3):
        with pytest.raises(ProtocolError) as e:
            dev.read_boot_region(region, 16)
        assert e.value.status == Status.FAULT
    with pytest.raises(ProtocolError) as e:
        dev.read_internal(0x08000000, 16)
    assert e.value.status == Status.FAULT
    with pytest.raises(ProtocolError) as e:
        dev.read_internal(0x20000000, 16)
    assert e.value.status == Status.RANGE
    cfg = dev.export_configuration()
    assert cfg['device']['product'].endswith('(simulated)') and len(bytes.fromhex(cfg['settings_hex'])) == 256
    assert 'option_bytes_hex' not in cfg
