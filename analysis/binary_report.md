# Binary forensics

Input: `DMT-99_V2.4_20240306.bin`, 135096 bytes (`0x20fb8`), SHA-256 `4befdf6b58e063f03901f190000d790e60d20d0a23e5eeb2b6f6840f1830e98e`.
The original is opened only for reading and marked read-only; no hardware is accessed.

## Verified execution layout

The first word `0x2680606f` is `jal x0,+0x6268`: application entry `0x6400` jumps to `0xc668`.
There is no observed update-container header: executable entry, vector pointers, startup code,
and SRAM initialization payload form a self-consistent raw application image. The lower
`0x6400` bytes of flash are absent; a resident bootloader there is a hypothesis, not a recovered component.

| Candidate base | Data destination from startup | Stack top | Vectors in code | Self-loop handlers |
|---|---|---|---:|---:|
| 0x0 | 0x1fff9c00 | 0x20009c00 | 86 | 0 |
| 0x6400 | 0x20000000 | 0x20010000 | 85 | 78 |
| 0x8000000 | 0x27ff9c00 | 0x28009c00 | 0 | 0 |
| 0x8006400 | 0x28000000 | 0x28010000 | 0 | 0 |

At base `0x6400`, AUIPC+ADDI sets gp=`0x200009b0`, sp=`0x20010000`,
copies `[0x271d0,0x273b8)` to `[0x20000000,0x200001e8)`, and zeros
`[0x200001e8,0x200054d8)`. It sets vendor CSRs `0xbc0=31`, `0x804=11`,
sets mstatus bits `0x6088`, mtvec=`0x643b`, calls `0xd212`, and uses mepc/mret to enter `0x908a`.
The gp, stack, SRAM copy, and absolute vector pointers jointly reject the other tested bases.
The SDK documents the `0x08000000` flash alias; low-window linkage is demonstrated by this binary.

## Instruction decoding

RV32 little endian, compressed instructions, and integer multiply/divide are present.
No hardware floating-point instructions are observed in recovered code; software floating-point
calls pass values in integer registers, supporting the ilp32 soft-float ABI. WCH Xwchc byte/halfword loads/stores overlap standard RVC double
floating-point instructions. A project-local SLEIGH processor corrects all eight Xwchc forms,
uses the target's 32-bit FPR definitions and selects a soft-float compiler spec. The original installed
Ghidra and firmware bytes are not patched. The decoder definitions follow LLVM PR 97925.

## Data and resources

The code extent ends at `0x1e820` (file `0x18420`); this is an extent, not a count of decoded
executable bytes. The remainder contains lookup/function-pointer tables, bitmap/font-like data,
UI strings, JPEG resource paths, and the SRAM load payload. ASCII strings and 1 KiB entropy
windows are exported in JSON. Many `0:/...jpg` paths indicate resources outside this BIN;
the supplied image alone cannot reconstruct the 8 MiB flash filesystem. No global encryption
is compatible with the verified plaintext executable. No validated image-wide CRC/checksum field
or version/date string has been identified; the filename alone is not proof of embedded version.
Individual compressed resources and font formats still require separate carving/format analysis.

## Primary references

- [WCH SDK, pinned revision](https://github.com/openwch/ch32v307/tree/080af2e52055334eaa6af0f7ad7df527b4ef5a66)
- [LLVM WCH instruction implementation](https://github.com/llvm/llvm-project/pull/97925)
- Local `references/wch-sdk-manifest.json` records downloaded source hashes.
