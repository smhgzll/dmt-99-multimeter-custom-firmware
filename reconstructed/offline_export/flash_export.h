#ifndef DMT_FLASH_EXPORT_H
#define DMT_FLASH_EXPORT_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* New experimental protocol, NOT an original firmware translation. No MMIO,
 * startup, linker placement, flash controller or physical transport binding.
 * Main Code Flash bounds from WCH RM 2.5 table 32-1; NOT system/option Flash.
 * A caller must also restrict the policy to its independently verified region.
 */
#define DMT_EXPORT_MAIN_BASE UINT32_C(0x08000000)
#define DMT_EXPORT_MAIN_END  UINT32_C(0x08078000)
#define DMT_EXPORT_BLOCK 256u
#define DMT_EXPORT_HEADER 32u
#define DMT_EXPORT_MAX_FRAME (DMT_EXPORT_HEADER + DMT_EXPORT_BLOCK + 4u)
enum { DMT_EXPORT_DATA=1, DMT_EXPORT_END=2, DMT_EXPORT_ERROR=3 };
enum { DMT_EXPORT_READ_FAILED=1, DMT_EXPORT_UNSTABLE=2 };
typedef struct {
    uint32_t allowed_start, allowed_bytes; /* immutable read policy */
    uint32_t start, bytes, session;
    uint8_t passes; /* 2..8; each block also read twice before transmission */
} dmt_export_plan;
typedef struct {
    void *context;
    /* Must read exactly n bytes or return false. Never supplied a write API. */
    bool (*read)(void *, uint32_t address, uint8_t *out, size_t n);
    bool (*send)(void *, const uint8_t *frame, size_t n);
} dmt_export_io;
uint32_t dmt_export_crc32(const uint8_t *,size_t);
bool dmt_export_plan_valid(const dmt_export_plan *);
/* Fail closed on invalid policy, any read mismatch/failure or send failure.
 * An ERROR frame is attempted for read failures; no END success follows it.
 * The caller, not this library, decides how/where execution could be started.
 */
bool dmt_flash_export(const dmt_export_plan *,const dmt_export_io *);
#endif
