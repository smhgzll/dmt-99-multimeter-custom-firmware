#ifndef DMT_MGMT_PROTOCOL_H
#define DMT_MGMT_PROTOCOL_H
/* POTAT-99 management protocol, version 1 (DMT99 Studio link).
 *
 * Transport independent. The host simulator serves it on a pseudo terminal
 * (simulated device for DMT99 Studio). The meter firmware has no transport
 * for it yet; the release image does not include it. Specification:
 * dmt99-studio/DEVICE_PROTOCOL.md.
 *
 * Request  D9 9A ver cmd seq16 len16 payload[len<=512] crc32
 * Response D9 9B ver cmd seq16 status len16 payload[len<=4096+64] crc32
 * All integers little endian; crc32 = IEEE 802.3 (zlib) over every byte
 * before it. Event frames (measurement stream) use the response layout
 * with cmd MGMT_MEASUREMENT_EVENT and their own sequence counter.
 *
 * Safety rules enforced here, not in the host:
 *  - Winbond writes only inside the assets partition (sectors 463..2047),
 *    only inside an open write session, and a page may only be programmed
 *    in the sector erased last in that session;
 *  - internal Flash and boot-region reads are read-only and go through the
 *    backend's fault-guarded reader; nothing here erases or programs the
 *    MCU's internal Flash, option bytes or calibration data;
 *  - settings changes are limited to documented fields and are kept in RAM
 *    (stored with the normal power-off commit, as menu changes are).
 */
#include <stdbool.h>
#include <stdint.h>

#define MGMT_PROTOCOL_VERSION 1u
#define MGMT_REQUEST_MAX 512u
#define MGMT_READ_MAX 4096u
#define MGMT_PROGRAM_MAX 256u
#define MGMT_SPI_SECTOR 4096u
#define MGMT_ASSET_FIRST_SECTOR 463u
#define MGMT_ASSET_SECTORS 1585u
#define MGMT_SESSION_TIMEOUT_MS 15000u
#define MGMT_FRAME_TIMEOUT_MS 1000u

enum mgmt_command {
    MGMT_PING = 0x00,
    MGMT_GET_DEVICE_INFO = 0x01,
    MGMT_GET_FIRMWARE_VERSION = 0x02,
    MGMT_GET_MEMORY_MAP = 0x03,
    MGMT_GET_CAPABILITIES = 0x04,
    MGMT_GET_SPI_FLASH_ID = 0x10,
    MGMT_READ_SPI_FLASH = 0x11,
    MGMT_READ_SPI_PARTITION = 0x12,
    MGMT_VERIFY_SPI_DATA = 0x13,
    MGMT_WRITE_SPI_ASSET_REGION = 0x14,
    MGMT_GET_CURRENT_MEASUREMENT = 0x20,
    MGMT_START_MEASUREMENT_STREAM = 0x21,
    MGMT_STOP_MEASUREMENT_STREAM = 0x22,
    MGMT_MEASUREMENT_EVENT = 0x23,
    MGMT_GET_SETTINGS = 0x30,
    MGMT_SET_SETTINGS = 0x31,
    MGMT_READ_ACCESSIBLE_INTERNAL_FLASH = 0x40,
    MGMT_READ_ACCESSIBLE_BOOT_REGION = 0x41,
    MGMT_EXPORT_CONFIGURATION = 0x42,
};

enum mgmt_status {
    MGMT_OK = 0,
    MGMT_E_FRAME = 1,        /* bad CRC, length or version */
    MGMT_E_COMMAND = 2,      /* unknown command */
    MGMT_E_ARGUMENT = 3,     /* malformed payload */
    MGMT_E_RANGE = 4,        /* address/length outside the allowed region */
    MGMT_E_DENIED = 5,       /* operation not allowed (write outside assets, no erase first) */
    MGMT_E_BUSY = 6,         /* another write session is open */
    MGMT_E_IO = 7,           /* flash driver reported an error / timeout */
    MGMT_E_VERIFY = 8,       /* programmed data did not read back */
    MGMT_E_UNAVAILABLE = 9,  /* e.g. no measurement while the charging screen is shown */
    MGMT_E_FAULT = 10,       /* the guarded reader trapped: region not readable */
    MGMT_E_SESSION = 11,     /* no open write session / wrong token / timed out */
};

/* WRITE_SPI_ASSET_REGION sub-operations (first payload byte). */
enum mgmt_write_op { MGMT_WRITE_BEGIN = 1, MGMT_WRITE_ERASE = 2, MGMT_WRITE_PROGRAM = 3, MGMT_WRITE_END = 4 };

/* SET_SETTINGS fields. */
enum mgmt_setting {
    MGMT_SET_LANGUAGE = 1,    /* 0 Chinese, 1 English */
    MGMT_SET_BACKLIGHT = 2,   /* 0..3 (settings word bits 3-4) */
    MGMT_SET_FAHRENHEIT = 3,  /* 0/1 */
    MGMT_SET_AUTO_OFF = 4,    /* 0 never, 1 5 min, 2 15 min, 3 30 min */
    MGMT_SET_VIEW = 5,        /* 1 regular, 2 recording, 3 monitor (applied at next start) */
    MGMT_SET_LAYOUT = 6,      /* 0 classic, 1 large digits (applied at next start) */
};

/* Boot-region ids for READ_ACCESSIBLE_BOOT_REGION. */
enum mgmt_boot_region {
    MGMT_BOOT_VENDOR_BOOTLOADER = 0, /* FNIRSI USB bootloader, code Flash 0x08000000..0x080063ff */
    MGMT_BOOT_SYSTEM_ROM = 1,        /* WCH system boot (ISP) area 0x1fff8000.. */
    MGMT_BOOT_OPTION_BYTES = 2,      /* 0x1ffff800, 16 bytes (read only) */
    MGMT_BOOT_ESIG = 3,              /* 0x1ffff7e0: capacity and unique ID */
};

typedef struct {
    const char *product, *version, *git, *build_date;
    uint32_t load_address, image_size, image_crc32;
    uint32_t chip_id, flash_kb, jedec, spi_capacity, uptime_ms;
    uint8_t uid[12];
    uint8_t clock_source, run_state, charging, battery;
    uint8_t rdp_protected;
} mgmt_device_info;

typedef struct {
    uint32_t frame_seq, frame_age_ms;
    uint8_t run_state, view, layout, mode, unit, numeric, hold, relative, ac_dc, status_high, charging, battery;
    float value, relative_value;
    char text[8];
    uint8_t frame[13];
} mgmt_measurement;

typedef struct {
    void *ctx;
    uint32_t (*now_ms)(void *);
    /* Blocking write of response bytes; false on timeout or disconnect. */
    bool (*write)(void *, const uint8_t *, uint16_t);
    void (*info)(void *, mgmt_device_info *);
    bool (*spi_read)(void *, uint32_t address, uint8_t *out, uint16_t n);
    bool (*spi_erase_sector)(void *, uint32_t sector);
    bool (*spi_program)(void *, uint32_t address, const uint8_t *data, uint16_t n);
    /* Fault-guarded read of MCU memory: MGMT_OK, MGMT_E_FAULT or MGMT_E_RANGE. */
    uint8_t (*memory_read)(void *, uint32_t address, uint8_t *out, uint16_t n);
    bool (*measurement)(void *, mgmt_measurement *);
    void (*settings_get)(void *, uint8_t out[256]);
    uint8_t (*settings_set)(void *, uint8_t field, uint16_t value);
    /* Asset write session: stop/resume UI access to the Winbond, remount after. */
    void (*maintenance)(void *, bool on);
} mgmt_backend;

typedef struct {
    const mgmt_backend *b;
    uint8_t rx[10 + MGMT_REQUEST_MAX + 4];
    uint16_t rx_len;
    uint32_t rx_started;
    bool session;
    uint32_t token, session_seen;
    int32_t erased_sector;
    bool stream;
    uint16_t interval_ms, event_seq;
    uint32_t last_event_ms, last_frame_seq;
    uint32_t frames_ok, frames_bad;
} mgmt_state;

void mgmt_init(mgmt_state *, const mgmt_backend *);
void mgmt_receive(mgmt_state *, const uint8_t *data, uint16_t n);
void mgmt_poll(mgmt_state *);
bool mgmt_maintenance_active(const mgmt_state *);
uint32_t mgmt_crc32(uint32_t crc, const uint8_t *data, uint32_t n);
#endif
