#ifndef DMT_SPI_FLASH_H
#define DMT_SPI_FLASH_H
#include <stddef.h>
#include "dmt_bus.h"
uint32_t spi_flash_read_jedec_id(const dmt_spi_bus *bus);
/* d782 capacity table over the JEDEC ID; 0 for an absent part (0 or ~0).
 * Unlisted IDs default to 2 MiB exactly as the original decision tree. The
 * original stores the result in globals that nothing reads. */
uint32_t spi_flash_capacity_from_jedec(uint32_t id);
uint8_t spi_flash_read_status(const dmt_spi_bus *bus);
void spi_flash_read(const dmt_spi_bus *bus, uint8_t *out, uint32_t address, uint16_t length);
size_t spi_flash_page_chunk(uint32_t address,size_t remaining);
/* Recovered write operations, 0xd4f8,0xd538,0xd588,0xd602,0xd6bc.
 * budget=0 preserves the original unlimited busy wait; a nonzero budget is an
 * explicit reconstruction guard. Only mocked buses are bound in the project. */
void spi_flash_write_enable(const dmt_spi_bus *);
bool spi_flash_erase_sector(const dmt_spi_bus *,uint32_t sector,uint32_t poll_budget);
bool spi_flash_page_program(const dmt_spi_bus *,const uint8_t *,uint32_t,uint16_t,uint32_t);
bool spi_flash_program_pages(const dmt_spi_bus *,const uint8_t *,uint32_t,uint16_t,uint32_t);
bool spi_flash_write_preserve_sector(const dmt_spi_bus *,const uint8_t *,uint32_t,uint16_t,uint8_t scratch[4096],uint32_t);
#endif
