# No target connection. Override CROSS for your CH32V303-compatible toolchain.
CROSS ?= $(shell python3 ../../../tools/find_riscv_toolchain.py)
ifeq ($(strip $(CROSS)),)
CROSS := riscv-none-elf-
endif
CC := $(CROSS)gcc
AR := $(CROSS)ar
SIZE := $(CROSS)size
BUILD ?= ../build
ARCH := -march=rv32imac_zicsr_zifencei -mabi=ilp32
CFLAGS := $(ARCH) -Os -std=gnu11 -ffreestanding -fno-builtin -fno-common -ffunction-sections -fdata-sections -Wall -Wextra -Werror
.PHONY: all
all: $(BUILD)/libdmt99_asic_rv32.a
$(BUILD)/dmt99_asic_rv32.o: dmt99_asic.c dmt99_asic.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -I. -c $< -o $@
$(BUILD)/libdmt99_asic_rv32.a: $(BUILD)/dmt99_asic_rv32.o
	$(AR) rcs $@ $<
	$(SIZE) $<
