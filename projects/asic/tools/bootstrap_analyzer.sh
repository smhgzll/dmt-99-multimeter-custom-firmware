#!/usr/bin/env bash
# Install only into this project. No target connection, no EEPROM programming.
set -euo pipefail
task_root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
mkdir -p "$task_root/.local/rpms" "$task_root/.local/extracted"
dnf download --destdir "$task_root/.local/rpms" sigrok-firmware-fx2lafw
for task_rpm in "$task_root"/.local/rpms/sigrok-firmware-fx2lafw-*.rpm; do
    rpm -K "$task_rpm"
    rpm2cpio "$task_rpm" | (cd "$task_root/.local/extracted" && cpio -idm --quiet)
done
python3 "$task_root/tools/dmt99.py" probe
