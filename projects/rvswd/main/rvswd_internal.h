// SPDX-License-Identifier: MIT
#pragma once
#include "rvswd_ro.h"
// Private to the transport and diagnostic engine. No host-selected DMI addresses.
rv_result_t rv_diag_begin(rv_bus_t *, unsigned, bool, bool);
rv_result_t rv_diag_preflight(rv_bus_t *, rv_identity_t *);
rv_result_t rv_diag_read(rv_bus_t *, uint8_t, uint32_t *);
rv_result_t rv_diag_write(rv_bus_t *, uint8_t, uint32_t);
bool rv_snapshot_address_allowed(uint32_t);
