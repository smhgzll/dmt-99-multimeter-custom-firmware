# Separate experiment: does not enter STOCK_COMPATIBLE or CUSTOM_DMT99 firmware.
HOST_CC ?= gcc
BUILD ?= ../build/offline_export
CFLAGS := -std=c11 -O2 -Wall -Wextra -Werror
.PHONY: all test sanitize
all: $(BUILD)/flash-export-replay
$(BUILD):
	mkdir -p $@
$(BUILD)/flash-export-replay: flash_export.c replay.c flash_export.h Makefile | $(BUILD)
	$(HOST_CC) $(CFLAGS) flash_export.c replay.c -o $@
$(BUILD)/test_flash_export: flash_export.c test_flash_export.c flash_export.h Makefile | $(BUILD)
	$(HOST_CC) $(CFLAGS) flash_export.c test_flash_export.c -o $@
test: all $(BUILD)/test_flash_export
	$(BUILD)/test_flash_export
	python3 ../../tools/test_flash_export.py --replay $(BUILD)/flash-export-replay
sanitize: | $(BUILD)
	clang -std=c11 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer flash_export.c test_flash_export.c -o $(BUILD)/test_flash_export_san
	ASAN_OPTIONS=detect_leaks=0:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 $(BUILD)/test_flash_export_san
