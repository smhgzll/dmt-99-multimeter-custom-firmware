#!/usr/bin/env python3
"""Find a local RV32IMAC-capable GCC prefix without machine-specific paths."""
import os
from pathlib import Path
import shutil


def find_prefix():
    if os.environ.get('CROSS'):
        return os.environ['CROSS']
    for prefix in ('riscv-none-elf-', 'riscv32-unknown-elf-', 'riscv32-esp-elf-',
                   'riscv64-unknown-elf-'):
        gcc = shutil.which(prefix + 'gcc')
        if gcc:
            return gcc.removesuffix('gcc')
    # Optional ESP-IDF tool installation, resolved against the current user's home.
    base = Path(os.environ.get('IDF_TOOLS_PATH', Path.home() / '.espressif'))
    for gcc in sorted((base / 'tools/riscv32-esp-elf').glob('*/riscv32-esp-elf/bin/riscv32-esp-elf-gcc'), reverse=True):
        return str(gcc).removesuffix('gcc')
    return 'riscv-none-elf-'


if __name__ == '__main__':
    print(find_prefix())
