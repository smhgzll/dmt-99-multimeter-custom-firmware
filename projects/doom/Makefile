SHELL := /bin/bash
ROOT := $(CURDIR)
PYTHON ?= python3
EMU_PYTHON ?= $(PYTHON)
HOST_CC ?= gcc
CROSS ?= $(shell python3 ../../tools/find_riscv_toolchain.py)
RVCC := $(CROSS)gcc
WAD ?= ../../artifacts/game-data/doom1.wad
ARCH := -march=rv32imac_zicsr_zifencei -mabi=ilp32
INC := -Isrc -Igenerated
HFLAGS := -std=c11 -O2 -Wall -Wextra -Werror $(INC)
FWFLAGS := $(ARCH) -Os -g -std=gnu11 -ffreestanding -fno-builtin -fno-common -ffunction-sections -fdata-sections -fstack-usage -MMD -MP \
 -Wall -Wextra -Werror -Wno-unused-parameter -DCH32V30x_D8 -DHSE_VALUE=8000000 -DDMT_FW_APP_IMAGE=1 \
 $(INC) -Iboard/reference -Ithird_party/wch/Core -Ithird_party/wch/Peripheral/inc
CORE := src/game.c src/render.c src/app.c src/sound.c src/controls.c generated/assets.c
HEADERS := $(wildcard src/*.h)
FW := $(CORE) board/board.c board/errno.c board/reference/system.c board/reference/delay.c board/reference/memory.c \
 board/reference/display.c board/reference/display_bus.c \
 $(addprefix third_party/wch/Peripheral/src/ch32v30x_,$(addsuffix .c,gpio rcc tim))
OBJ := $(addprefix build/fw/,$(FW:.c=.o)) build/fw/board/reference/startup.o
ELF := build/dmt99-doom.elf
.PHONY: all assets firmware simulator test sanitize emulate verify package clean
all: firmware simulator
assets: generated/assets.c
generated/assets.c generated/assets.h generated/map_data.h &: tools/build_assets.py $(WAD) upstream/doom-avr/host/map_data.py
	PYTHONDONTWRITEBYTECODE=1 $(PYTHON) tools/build_assets.py $(WAD)
build/fw/%.o: %.c generated/assets.h generated/map_data.h
	@mkdir -p $(@D)
	$(RVCC) $(FWFLAGS) -c $< -o $@
build/fw/%.o: %.S
	@mkdir -p $(@D)
	$(RVCC) $(FWFLAGS) -c $< -o $@
$(ELF): $(OBJ) board/reference/dmt99-app.ld board/reference/dmt99-sections.ld
	$(RVCC) $(ARCH) -nostdlib -Wl,--gc-sections,-Map,build/dmt99-doom.map -Wl,-L,board/reference -Tboard/reference/dmt99-app.ld $(OBJ) -lm -lgcc -o $@
build/dmt99-doom.bin: $(ELF)
	$(CROSS)objcopy -O binary $< $@
	$(CROSS)size $< > build/dmt99-doom.size.txt
	$(CROSS)objdump -d $< > build/dmt99-doom.lst
	PYTHONDONTWRITEBYTECODE=1 $(PYTHON) tools/check_image.py --cross '$(CROSS)'
	PYTHONDONTWRITEBYTECODE=1 $(PYTHON) tools/stack_report.py $(ELF) $(CROSS)objdump build/fw 6144 > build/stack.txt
firmware: build/dmt99-doom.bin
build/doom-host: $(CORE) src/host.c $(HEADERS) generated/assets.h generated/map_data.h
	@mkdir -p build
	$(HOST_CC) $(HFLAGS) $(CORE) src/host.c -lm -o $@
build/libdoom-host.so: $(CORE) src/host.c $(HEADERS) generated/assets.h generated/map_data.h
	@mkdir -p build
	$(HOST_CC) $(HFLAGS) -fPIC -shared -DDOOM_HOST_LIBRARY $(CORE) src/host.c -lm -o $@
simulator: build/doom-host build/libdoom-host.so
build/test-core: $(CORE) tests/test_core.c $(HEADERS) generated/assets.h generated/map_data.h
	@mkdir -p build
	$(HOST_CC) $(HFLAGS) $(CORE) tests/test_core.c -lm -o $@
build/test-sound: src/sound.c tests/test_sound.c $(HEADERS)
	@mkdir -p build
	$(HOST_CC) $(HFLAGS) src/sound.c tests/test_sound.c -o $@
build/test-events: src/game.c tests/test_events.c generated/assets.c generated/map_data.h $(HEADERS)
	@mkdir -p build
	$(HOST_CC) $(HFLAGS) tests/test_events.c generated/assets.c -lm -o $@
build/test-controls: src/controls.c tests/test_controls.c $(HEADERS)
	@mkdir -p build
	$(HOST_CC) $(HFLAGS) src/controls.c tests/test_controls.c -o $@
test: build/test-core build/test-sound build/test-events build/test-controls build/doom-host build/libdoom-host.so
	build/test-core
	build/test-sound
	build/test-events
	build/test-controls
	PYTHONDONTWRITEBYTECODE=1 $(PYTHON) tests/test_host.py
	PYTHONDONTWRITEBYTECODE=1 $(PYTHON) tools/test_upstream.py
sanitize: generated/assets.c
	@mkdir -p build
	clang $(HFLAGS) -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer $(CORE) tests/test_core.c -lm -o build/test-core-sanitize
	ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 build/test-core-sanitize
	clang $(HFLAGS) -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer src/sound.c tests/test_sound.c -o build/test-sound-sanitize
	ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 build/test-sound-sanitize
	clang $(HFLAGS) -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer tests/test_events.c generated/assets.c -lm -o build/test-events-sanitize
	ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 build/test-events-sanitize
	clang $(HFLAGS) -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer src/controls.c tests/test_controls.c -o build/test-controls-sanitize
	ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 build/test-controls-sanitize
emulate: all
	PYTHONDONTWRITEBYTECODE=1 $(EMU_PYTHON) tools/test_emulator.py
verify:
	PYTHONDONTWRITEBYTECODE=1 $(PYTHON) tools/check_reference.py
package:
	PYTHONDONTWRITEBYTECODE=1 $(PYTHON) tools/package.py
clean:
	rm -rf build generated
-include $(OBJ:.o=.d)
