/* Host-portable integer binding for ChaN TJpgDec R0.01b.
 * The upstream integer.h requires LONG/DWORD to be exactly 32 bits.
 * RV32 ILP32 layout is unchanged; LP64 host tests use fixed-width types. */
#ifndef DMT_TJPG_INTEGER_H
#define DMT_TJPG_INTEGER_H
#include <stdint.h>
typedef int INT;
typedef unsigned int UINT;
typedef char CHAR;
typedef unsigned char UCHAR;
typedef uint8_t BYTE;
typedef int16_t SHORT;
typedef uint16_t USHORT;
typedef uint16_t WORD;
typedef uint16_t WCHAR;
typedef int32_t LONG;
typedef uint32_t ULONG;
typedef uint32_t DWORD;
#endif
