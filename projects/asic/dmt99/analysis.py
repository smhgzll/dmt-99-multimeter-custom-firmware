"""Descriptive protocol evidence and explicitly unconfirmed hypotheses."""
from collections import Counter, defaultdict
from functools import reduce
import math
import operator
import statistics


def burst_statistics(result):
    """Idle-delimited wire observations independent of the chosen frame length."""
    report = {}
    threshold = result["gap_chars"] * 10 * result["samplerate_hz"] / result["baud"]
    for direction in ("MCU_TX", "ASIC_TX"):
        records = [r for r in result["bytes"] if r["direction"] == direction]
        bursts = []
        for r in records:
            if not bursts or r["start_sample"] - bursts[-1][-1]["end_sample_exclusive"] > threshold:
                bursts.append([])
            bursts[-1].append(r)
        periods = [(b[0]["start_sample"] - a[0]["start_sample"]) / result["samplerate_hz"]
                   for a, b in zip(bursts, bursts[1:])]
        report[direction] = {"gap_threshold_samples": threshold, "burst_count": len(bursts),
                             "uart_candidates_per_burst": dict(Counter(map(len, bursts))),
                             "start_period_s": None if not periods else {
                                 "min": min(periods), "median": statistics.median(periods), "max": max(periods)}}
    return report


def checksum_hypotheses(packets):
    # Search finite, documented candidates; do not infer CRC from a constant frame.
    frames = [p["values"] for p in packets if p["complete"] and not p["uart_errors"]]
    lengths = sorted({len(f) for f in frames})
    if len(lengths) > 1:
        return {"status": "hypotheses_only_separate_lengths", "by_wire_length": {
            str(n): checksum_hypotheses([p for p in packets if p["complete"] and len(p["values"]) == n])
            for n in lengths}}
    unique = sorted(set(tuple(f) for f in frames))
    prefix_outputs = defaultdict(set)
    for frame in unique: prefix_outputs[frame[:-1]].add(frame[-1])
    contradictions = [{"identical_prefix": list(prefix), "observed_last_bytes": sorted(values)}
                      for prefix, values in prefix_outputs.items() if len(values) > 1]
    candidates = []
    if not unique: return {"status": "no_clean_frames", "candidates": []}
    length = len(unique[0])
    for target in range(2, length):
        for start in range(0, target):
            for name in ("sum8", "negative_sum8", "xor8"):
                matches = 0
                for frame in unique:
                    region = frame[start:target]
                    value = reduce(operator.xor, region, 0) if name == "xor8" else sum(region)
                    if name == "negative_sum8": value = -value
                    matches += (value & 255) == frame[target]
                if matches == len(unique):
                    candidates.append({"algorithm": name, "input_start": start,
                                       "input_end_exclusive": target, "target_offset": target,
                                       "matching_distinct_frames": matches, "confirmed": False,
                                       "informative_input_variation": len({tuple(f[start:target]) for f in unique}) > 1,
                                       "target_varies": len({f[target] for f in unique}) > 1})
    return {"status": "hypotheses_only", "frames": len(frames), "distinct_frames": len(unique),
            "last_byte_deterministic_checksum_of_prefix": "contradicted_in_capture" if contradictions else "unresolved",
            "identical_prefix_different_last_byte": contradictions,
            "search": "sum8/negative_sum8/xor8 over contiguous prefix ending before target; no CRC claim",
            "candidates": candidates}


def summarize(result):
    groups = defaultdict(list)
    repetitions = Counter()
    for packet in result["packets"]:
        repetitions[(packet["direction"], packet["hex"], packet["kind"])] += 1
        if not packet["complete"]: continue
        if packet["direction"] == "MCU_TX": key = f"MCU_TX:command_{packet['command_id']:02X}"
        else:
            d = packet["firmware_interpretation"]
            key = "ASIC_TX:" + (f"event_{d['event_id']:02X}" if d["kind"] == "event" else "display")
        groups[key].append(packet)
    grouped = {}
    for key, items in groups.items():
        columns = list(zip(*(p["values"] for p in items)))
        grouped[key] = {"count": len(items), "distinct_frames": len({p["hex"] for p in items}),
                        "uart_error_frames": sum(bool(p["uart_errors"]) for p in items),
                        "byte_values": {str(i): sorted(set(c)) for i, c in enumerate(columns)},
                        "changing_offsets": [i for i, c in enumerate(columns) if len(set(c)) > 1],
                        "first_t_s": items[0]["t_s"], "last_t_s": items[-1]["t_s"]}
    rx = [p for p in result["packets"] if p["direction"] == "ASIC_TX"]
    readings = Counter((p["firmware_interpretation"]["display_text"],
                        p["firmware_interpretation"]["unit_resource_association"])
                       for p in rx if p.get("firmware_interpretation", {}).get("kind") == "display")
    return {"label": result["metadata"].get("label"), "origin": result["origin"],
            "wire_burst_observations": burst_statistics(result),
            "total_uart_candidates": len(result["bytes"]),
            "uart_error_candidates": sum(bool(r["errors"]) for r in result["bytes"]),
            "packet_counts": dict(Counter(p["kind"] for p in result["packets"])),
            "groups": grouped,
            "repeated_messages": [{"direction": d, "hex": h, "kind": k, "count": n}
                                  for (d, h, k), n in repetitions.most_common()],
            "firmware_display_histogram": [{"text": t, "unit": u, "count": n}
                                            for (t, u), n in readings.most_common()],
            "rx_checksum_hypotheses": checksum_hypotheses(rx),
            "evidence_state": {"confirmed_hardware_fields": [],
                               "firmware_derived": ["packed_segments", "sign", "decimal_priority", "unit_priority", "FD_event"],
                               "unknown": ["RX_checksum", "b9..12", "physical_range", "ASIC_identity", "event_ACK_causality"],
                               "note": "Repeated observations and matching checksums alone do not confirm semantics."}}


def compare(results):
    if any(r["origin"] != "hardware" for r in results):
        raise ValueError("Cross-physical-test comparison accepts hardware-origin sessions only")
    observations = []
    for result in results:
        metadata = result["metadata"]
        expected = metadata.get("display_numeric")
        unit = metadata.get("display_unit")
        tolerance = metadata.get("display_tolerance", 0)
        approximate_without_tolerance = metadata.get("display_is_approximate", False) and not tolerance
        eligible, matches = 0, 0
        for p in result["packets"]:
            d = p.get("firmware_interpretation", {})
            if p["uart_errors"] or d.get("kind") != "display": continue
            if d["numeric_value"] is None or expected is None or unit is None or approximate_without_tolerance: continue
            eligible += 1
            matches += (math.isclose(d["numeric_value"], expected, rel_tol=0, abs_tol=tolerance)
                        and d["unit_resource_association"] == unit)
        observations.append({"label": metadata.get("label"), "metadata": metadata,
                             "eligible_clean_numeric_frames": eligible,
                             "display_numeric_and_unit_matches": matches,
                             "comparison_policy": "explicit_tolerance_required_for_approximate_target" if approximate_without_tolerance else "numeric_and_unit_with_recorded_tolerance",
                             "status": "untested" if not eligible else "matches_observed" if matches else "mismatch_observed",
                             "semantic_confirmation": False,
                             "analysis": summarize(result)})
    return {"schema": 1, "sessions": observations,
            "next_tests": ["Repeat with another known component/value and label display metadata",
                           "Check open/closed and range transitions separately",
                           "Test checksum candidates on held-out physically different captures"],
            "automatic_confirmation": False}
