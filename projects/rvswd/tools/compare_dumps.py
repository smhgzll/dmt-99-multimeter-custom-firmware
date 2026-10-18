#!/usr/bin/env python3
"""Offline two-pass 256 KiB comparison only. Does not contact an adapter or target."""
import argparse
import hashlib
import json
from pathlib import Path

FLASH_BYTES = 256 * 1024


def digest(path):
    h = hashlib.sha256()
    size = 0
    with path.open("rb") as stream:
        while chunk := stream.read(4096):
            size += len(chunk)
            h.update(chunk)
    if size != FLASH_BYTES:
        raise ValueError(f"{path}: expected {FLASH_BYTES} bytes, got {size}")
    return h.hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("first", type=Path)
    parser.add_argument("second", type=Path)
    args = parser.parse_args()
    try:
        if args.first.samefile(args.second):
            raise ValueError("Two distinct independently acquired files are required")
        a, b = digest(args.first), digest(args.second)
    except (OSError, ValueError) as exc:
        parser.error(str(exc))
    print(json.dumps({"bytes_per_pass": FLASH_BYTES, "sha256_first": a,
                      "sha256_second": b, "match": a == b,
                      "acquisition_provenance_verified": False}, indent=2))
    return 0 if a == b else 1


if __name__ == "__main__":
    raise SystemExit(main())
