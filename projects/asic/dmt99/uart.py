"""9600 8N1 decoder; integer sample indices are the authoritative timestamps."""
import math
import re


def decode(data, samplerate, channel, baud=9600, block_size=1 << 20, direction=None):
    if samplerate < baud * 16 or channel not in range(8):
        raise ValueError("Need >=16 samples/bit and a channel bit in 0..7")
    bit = samplerate / baud
    table = bytes((n >> channel) & 1 for n in range(256))
    records = []
    cursor = 0
    previous = b"\x01"  # Start-low at sample 0 is explicitly ambiguous.
    edges = 0
    highs = 0
    for base in range(0, len(data), block_size):
        levels = bytes(data[base:base + block_size]).translate(table)
        highs += levels.count(1)
        window = previous + levels
        for match in re.finditer(b"\x01\x00", window):
            start = base + match.start()
            edges += 1
            if start < cursor:
                continue
            flags = []
            if start == 0:
                flags.append("capture_started_low_ambiguous_start")
            def sample(offset):
                index = start + int(math.floor(offset * bit + 0.5))
                return ((data[index] >> channel) & 1) if index < len(data) else None
            start_bit = sample(0.5)
            bits = [sample(1.5 + n) for n in range(8)]
            stop_bit = sample(9.5)
            if start_bit == 1:
                flags.append("short_low_false_start")
                cursor = start + max(1, int(bit / 2))
            else:
                if stop_bit == 0:
                    flags.append("framing_error_stop_low")
                if stop_bit is None:
                    flags.append("truncated_uart_frame")
                cursor = start + max(1, math.ceil(9.5 * bit) + 1)
            partial = sum((value or 0) << n for n, value in enumerate(bits))
            value = partial if None not in bits and start_bit == 0 else None
            records.append({
                "id": len(records), "channel": channel,
                "direction": direction or ("MCU_TX" if channel == 0 else "ASIC_TX"),
                "start_sample": start,
                "end_sample_exclusive": min(len(data), start + math.ceil(10 * bit)),
                "t_s": start / samplerate,
                "value": value, "hex": f"{value:02X}" if value is not None else None,
                "partial_value": partial, "bits_lsb_first": bits,
                "start_bit": start_bit, "stop_bit": stop_bit, "errors": flags,
            })
        previous = levels[-1:] or previous
    return records, {"samples": len(data), "high_samples": highs,
                     "low_samples": len(data) - highs, "falling_edges": edges,
                     "initial_level": ((data[0] >> channel) & 1) if len(data) else None,
                     "final_level": ((data[-1] >> channel) & 1) if len(data) else None}
