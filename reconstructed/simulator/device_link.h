#ifndef DMT_SIM_DEVICE_LINK_H
#define DMT_SIM_DEVICE_LINK_H
/* Simulated management link on a pseudo terminal (host only). */
#include "simulator.h"
#include "mgmt_protocol.h"
#include <stddef.h>

typedef struct {
    dmt_simulator *sim;
    mgmt_backend backend;
    mgmt_state state;
    int master, slave;
    char path[64];
    uint8_t *mcu_image;
    size_t mcu_image_size;
    uint32_t started, last_packet_ms, erases, programs;
    bool maintenance;
} device_link;

bool device_link_open(device_link *, dmt_simulator *, const char *mcu_image);
/* Waits up to timeout_ms for host bytes (and optionally stdin), services the protocol. */
void device_link_service(device_link *, int timeout_ms, int stdin_fd, bool *stdin_ready);
void device_link_packet(device_link *);
void device_link_close(device_link *);
#endif
