# Offline build only. There are deliberately no programmer or flashing targets.
SHELL := /bin/bash
ROOT := $(abspath ..)
SDK := $(CURDIR)/vendor/wch/EVT/EXAM/SRC
SDK_USER := $(CURDIR)/vendor/wch/EVT/EXAM/GPIO/GPIO_Toggle/User
CROSS ?= $(shell python3 $(ROOT)/tools/find_riscv_toolchain.py)
CC_RV := $(CROSS)gcc
OBJCOPY := $(CROSS)objcopy
OBJDUMP := $(CROSS)objdump
SIZE := $(CROSS)size
AR := $(CROSS)ar
HOST_CC ?= gcc
SANITIZER_CC ?= clang
CONFIG ?= STOCK_COMPATIBLE
ifeq ($(CONFIG),CUSTOM_DMT99)
BUILD := build/custom
PROFILE_FLAGS := -DCUSTOM_DMT99=1
else ifeq ($(CONFIG),STOCK_COMPATIBLE)
BUILD := build
PROFILE_FLAGS := -DSTOCK_COMPATIBLE=1
else
$(error CONFIG must be STOCK_COMPATIBLE or CUSTOM_DMT99)
endif
HEADERS := $(wildcard vendor/fatfs/*.h vendor/tjpgd/*.h) $(wildcard include/*.h drivers/*.h ui/*.h storage/*.h measurement/*.h application/*.h platform/*.h)
INCLUDES := -Ivendor/fatfs -Ivendor/tjpgd -Iinclude -Idrivers -Iui -Istorage -Imeasurement -Iapplication -Iplatform -I$(SDK)/Core -I$(SDK)/Peripheral/inc -I$(SDK_USER)
ARCH := -march=rv32imac_zicsr_zifencei -mabi=ilp32
CFLAGS := $(PROFILE_FLAGS) $(ARCH) -Os -g -ffreestanding -fno-builtin -ffunction-sections -fdata-sections -Wall -Wextra -MMD -MP -DCH32V30x_D8 -DHSE_VALUE=8000000 $(INCLUDES)
MODULES := ui/simple_screen.c ui/theme.c ui/theme_boot.c ui/theme_menu.c application/battery_level.c drivers/asic_driver.c drivers/measurement_reading.c ui/menu_render.c ui/settings_scene.c ui/units.c ui/screen.c storage/codepage.c ui/history_view.c ui/menu_scene.c ui/monitor.c application/last_mode.c ui/reading.c ui/view.c ui/gauge.c ui/scene_rom.c ui/graph.c ui/bitmap_images.c ui/resource_volume.c ui/numeric_view.c ui/resources.c storage/filesystem.c vendor/fatfs/ff.c vendor/fatfs/unicode_upper.c vendor/tjpgd/tjpgd.c drivers/measurement_asic.c drivers/spi_flash.c drivers/display_bus.c drivers/display.c ui/buttons.c ui/menu.c ui/status_view.c ui/graphics.c ui/chinese.c ui/font_rom.c storage/history.c storage/history_browser.c storage/disk.c storage/settings.c application/foreground.c application/control.c application/application.c application/format.c application/power.c storage/persistence.c measurement/value.c measurement/events.c measurement/thresholds.c
SDK_MODULES := gpio rcc usart dma spi exti tim adc misc flash
SDK_SOURCES := $(addprefix $(SDK)/Peripheral/src/ch32v30x_,$(addsuffix .c,$(SDK_MODULES)))
SOURCES := $(MODULES) platform/wch_platform.c platform/wch_runtime.c platform/wch_nvic.c platform/memory.c application/main.c $(SDK)/Core/core_riscv.c $(SDK_SOURCES)
OBJECTS := $(addprefix $(BUILD)/,$(notdir $(SOURCES:.c=.o))) $(BUILD)/startup.o $(BUILD)/system_ch32v30x.o
PHONY_STUDIO := studio-tools
.PHONY: $(PHONY_STUDIO) all host-test host-sanitize modules clean FORCE profile-test profile-sanitize simulator simulator-release simulator-test simulator-sanitize check asic-test asic-sanitize
all: $(BUILD)/reconstruction.elf $(BUILD)/reconstruction.disasm $(BUILD)/recovered_modules.elf $(BUILD)/recovered_modules.disasm $(BUILD)/libreconstructed.a
$(BUILD):
	mkdir -p $(BUILD)
define C_RULE
$(BUILD)/$(notdir $(1:.c=.o)): $(1) $(BUILD)/compile_flags | $(BUILD)
	$(CC_RV) $(CFLAGS) -c $$< -o $$@
endef
FORCE:
$(BUILD)/compile_flags: FORCE | $(BUILD)
	@python3 ../tools/config_stamp.py $@ '$(CC_RV)' '$(CFLAGS)'
$(foreach s,$(SOURCES),$(eval $(call C_RULE,$(s))))
$(BUILD)/system_ch32v30x_144.c: $(SDK_USER)/system_ch32v30x.c | $(BUILD)
	python3 -c 'from pathlib import Path;p=Path("$<").read_text();p=p.replace("#define SYSCLK_FREQ_96MHz_HSE  96000000", "/* 96 MHz disabled: original 0xd212 selects 144 MHz */");p=p.replace("//#define SYSCLK_FREQ_144MHz_HSE  144000000", "#define SYSCLK_FREQ_144MHz_HSE  144000000");Path("$@").write_text(p)'
$(BUILD)/system_ch32v30x.o: $(BUILD)/system_ch32v30x_144.c $(BUILD)/compile_flags
	$(CC_RV) $(CFLAGS) -c $< -o $@
$(BUILD)/startup.o: $(SDK)/Startup/startup_ch32v30x_D8.S $(BUILD)/compile_flags | $(BUILD)
	$(CC_RV) $(ARCH) -g -c $< -o $@
$(BUILD)/reconstruction.elf: $(OBJECTS) startup/Link.ld
	$(CC_RV) $(ARCH) -nostdlib -Tstartup/Link.ld -Wl,--gc-sections,-Map=$(BUILD)/reconstruction.map $(OBJECTS) -lgcc -o $@
	$(SIZE) $@
$(BUILD)/reconstruction.disasm: $(BUILD)/reconstruction.elf
	$(OBJDUMP) -d $< > $@
# Retain every maintained module and compiled SDK routine for review. The main
# entry remains the offline replay; this does not claim a complete device app.
$(BUILD)/recovered_modules.elf: $(OBJECTS) startup/Link.ld
	$(CC_RV) $(ARCH) -nostdlib -Tstartup/Link.ld -Wl,--no-gc-sections,-Map=$(BUILD)/recovered_modules.map $(OBJECTS) -lgcc -o $@
	$(SIZE) $@
$(BUILD)/recovered_modules.disasm: $(BUILD)/recovered_modules.elf
	$(OBJDUMP) -d $< > $@
modules: $(OBJECTS)
$(BUILD)/libreconstructed.a: $(addprefix $(BUILD)/,$(notdir $(MODULES:.c=.o))) $(BUILD)/wch_platform.o $(BUILD)/wch_runtime.o $(BUILD)/wch_nvic.o $(BUILD)/memory.o
	$(AR) rcs $@ $^
$(BUILD)/test_recovered: tests/test_recovered.c $(MODULES) $(HEADERS) Makefile | $(BUILD)
	$(HOST_CC) $(PROFILE_FLAGS) -std=c11 -O2 -Wall -Wextra -Werror $(INCLUDES) $(filter %.c,$^) -o $@
$(BUILD)/librecovered.so: $(MODULES) $(HEADERS) Makefile | $(BUILD)
	$(HOST_CC) $(PROFILE_FLAGS) -std=c11 -O2 -shared -fPIC -Wall -Wextra -Werror $(INCLUDES) $(filter %.c,$^) -o $@
$(BUILD)/test_application: tests/test_application.c $(MODULES) $(HEADERS) Makefile | $(BUILD)
	$(HOST_CC) $(PROFILE_FLAGS) -std=c11 -O2 -Wall -Wextra -Werror $(INCLUDES) $(filter %.c,$^) -o $@
host-test: $(BUILD)/test_resources $(BUILD)/test_recovered $(BUILD)/test_application $(BUILD)/librecovered.so
	$(BUILD)/test_recovered
	$(BUILD)/test_application
	$(BUILD)/test_resources ../flashdump/dmt99_flash_1.bin tests/flash_paths.txt
$(BUILD)/test_application_sanitized: tests/test_application.c $(MODULES) $(HEADERS) Makefile | $(BUILD)
	$(SANITIZER_CC) $(PROFILE_FLAGS) -std=c11 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer $(INCLUDES) $(filter %.c,$^) -o $@
host-sanitize: $(BUILD)/test_resources_sanitized $(BUILD)/test_application_sanitized
	ASAN_OPTIONS=detect_leaks=0:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 $(BUILD)/test_application_sanitized
	ASAN_OPTIONS=detect_leaks=0:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 $(BUILD)/test_resources_sanitized ../flashdump/dmt99_flash_1.bin tests/flash_paths.txt
clean:
	rm -rf $(BUILD)

-include $(wildcard $(BUILD)/*.d)

$(BUILD)/test_resources: tests/test_resources.c $(MODULES) $(HEADERS) Makefile | $(BUILD)
	$(HOST_CC) $(PROFILE_FLAGS) -std=c11 -O2 -Wall -Wextra -Werror $(INCLUDES) $(filter %.c,$^) -o $@
$(BUILD)/test_resources_sanitized: tests/test_resources.c $(MODULES) $(HEADERS) Makefile | $(BUILD)
	$(SANITIZER_CC) $(PROFILE_FLAGS) -std=c11 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer $(INCLUDES) $(filter %.c,$^) -o $@

$(BUILD)/test_last_mode: tests/test_last_mode.c $(MODULES) $(HEADERS) Makefile | $(BUILD)
	$(HOST_CC) $(PROFILE_FLAGS) -std=c11 -O2 -Wall -Wextra -Werror $(INCLUDES) $(filter %.c,$^) -o $@
profile-test: $(BUILD)/test_last_mode
	$(BUILD)/test_last_mode
$(BUILD)/test_last_mode_sanitized: tests/test_last_mode.c $(MODULES) $(HEADERS) Makefile | $(BUILD)
	$(SANITIZER_CC) $(PROFILE_FLAGS) -std=c11 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer $(INCLUDES) $(filter %.c,$^) -o $@
profile-sanitize: $(BUILD)/test_last_mode_sanitized
	ASAN_OPTIONS=detect_leaks=0:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 $(BUILD)/test_last_mode_sanitized

# Native host application. Never included in the RV32 image or device bindings.
SIM_SOURCES := simulator/simulator.c simulator/png.c simulator/device_link.c application/mgmt_protocol.c
$(BUILD)/dmt99-simulator: simulator/main.c $(SIM_SOURCES) $(MODULES) $(HEADERS) simulator/simulator.h simulator/png.h simulator/device_link.h application/mgmt_protocol.h Makefile | $(BUILD)
	$(HOST_CC) $(PROFILE_FLAGS) -std=c11 -O2 -Wall -Wextra -Werror $(INCLUDES) -Isimulator $(filter %.c,$^) -o $@
simulator: $(BUILD)/dmt99-simulator
# Host simulator with the release features (last mode, About label).
$(BUILD)/dmt99-simulator-release: simulator/main.c $(SIM_SOURCES) $(MODULES) $(HEADERS) simulator/simulator.h simulator/png.h simulator/device_link.h application/mgmt_protocol.h Makefile | $(BUILD)
	$(HOST_CC) $(PROFILE_FLAGS) -DDMT_RELEASE_FEATURES=1 '-DDMT_FW_VERSION="$(FW_VERSION)"' -std=c11 -O2 -Wall -Wextra -Werror $(INCLUDES) -Isimulator $(filter %.c,$^) -o $@
simulator-release: $(BUILD)/dmt99-simulator-release
# DMT99 Studio host tools: the simulator pair plus the device JPEG decoder check.
$(BUILD)/dmt99-jpeg-check: simulator/jpeg_check.c vendor/tjpgd/tjpgd.c vendor/tjpgd/tjpgd.h ui/resources.h Makefile | $(BUILD)
	$(HOST_CC) $(PROFILE_FLAGS) -std=c11 -O2 -Wall -Wextra -Werror $(INCLUDES) simulator/jpeg_check.c vendor/tjpgd/tjpgd.c -o $@
studio-tools: $(BUILD)/dmt99-simulator $(BUILD)/dmt99-simulator-release $(BUILD)/dmt99-jpeg-check
$(BUILD)/test_simulator: tests/test_simulator.c $(SIM_SOURCES) $(MODULES) $(HEADERS) simulator/simulator.h simulator/png.h simulator/device_link.h application/mgmt_protocol.h Makefile | $(BUILD)
	$(HOST_CC) $(PROFILE_FLAGS) -std=c11 -O2 -Wall -Wextra -Werror $(INCLUDES) -Isimulator $(filter %.c,$^) -o $@
simulator-test: $(BUILD)/test_simulator
	$(BUILD)/test_simulator ../flashdump/dmt99_flash_1.bin
$(BUILD)/test_simulator_sanitized: tests/test_simulator.c $(SIM_SOURCES) $(MODULES) $(HEADERS) simulator/simulator.h simulator/png.h simulator/device_link.h application/mgmt_protocol.h Makefile | $(BUILD)
	$(SANITIZER_CC) $(PROFILE_FLAGS) -std=c11 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer $(INCLUDES) -Isimulator $(filter %.c,$^) -o $@
simulator-sanitize: $(BUILD)/test_simulator_sanitized
	ASAN_OPTIONS=detect_leaks=0:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 $(BUILD)/test_simulator_sanitized ../flashdump/dmt99_flash_1.bin
check: host-test profile-test simulator-test

# Portable peer driver: no application, graphics, filesystem or WCH objects.
ASIC_SOURCES := drivers/asic_driver.c drivers/measurement_asic.c drivers/measurement_reading.c
$(BUILD)/test_asic_driver: tests/test_asic_driver.c $(ASIC_SOURCES) drivers/asic_driver.h drivers/measurement_asic.h ui/buttons.h Makefile | $(BUILD)
	$(HOST_CC) -std=c11 -O2 -Wall -Wextra -Werror -Idrivers -Iui $(filter %.c,$^) -o $@
asic-test: $(BUILD)/test_asic_driver
	$(BUILD)/test_asic_driver
$(BUILD)/test_asic_driver_sanitized: tests/test_asic_driver.c $(ASIC_SOURCES) drivers/asic_driver.h drivers/measurement_asic.h ui/buttons.h Makefile | $(BUILD)
	$(SANITIZER_CC) -std=c11 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -Idrivers -Iui $(filter %.c,$^) -o $@
asic-sanitize: $(BUILD)/test_asic_driver_sanitized
	ASAN_OPTIONS=detect_leaks=0:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 $(BUILD)/test_asic_driver_sanitized
$(BUILD)/asic_application_probe.so: tests/asic_application_probe.c $(MODULES) $(HEADERS) Makefile | $(BUILD)
	$(HOST_CC) $(PROFILE_FLAGS) -std=c11 -O2 -shared -fPIC -Wall -Wextra -Werror $(INCLUDES) $(filter %.c,$^) -o $@
$(BUILD)/asic-replay: ../examples/asic_driver_replay.c $(ASIC_SOURCES) drivers/asic_driver.h drivers/measurement_asic.h ui/buttons.h Makefile | $(BUILD)
	$(HOST_CC) -std=c11 -O2 -Wall -Wextra -Werror -Idrivers -Iui $(filter %.c,$^) -o $@

# ---- CH32V303RCT6 firmware ----------------------------------------------------
# FW_LAYOUT=app (USB update image, the release format): raw application BIN
# linked to 0x6400 with the V2.4 update-file header, installed by the meter's
# own bootloader (POWER+MENU, copy to the USB disk); the bootloader is kept.
# FW_LAYOUT=standalone: complete image for Code Flash 0x08000000 that replaces
# the bootloader (needs RDP removal and a debug probe).
# There are deliberately no programming targets; see final/README.md and
# docs/FIRMWARE_RELEASE.md before writing any device.
FW_BUILD ?= $(BUILD)/firmware
FW_NAME ?= dmt99-reconstructed
FW_LAYOUT ?= standalone
# Semantic version of the release (final/v$(FW_VERSION)); 0.y.z until the
# firmware has been validated on hardware.
FW_VERSION ?= 0.6.0
FW_DATE ?= $(shell date +%Y%m%d)
FW_GIT := $(shell $(ROOT)/tools/milestone rev-parse --short HEAD 2>/dev/null || echo unknown)
FW_DEFS ?=
ifeq ($(FW_LAYOUT),app)
FW_LDSCRIPT := firmware/dmt99-app.ld
FW_LAYOUT_DEFS := -DDMT_FW_APP_IMAGE=1 -DDMT_FW_LOAD_ADDRESS=0x08006400u
else ifeq ($(FW_LAYOUT),standalone)
FW_LDSCRIPT := firmware/dmt99.ld
FW_LAYOUT_DEFS :=
else
$(error FW_LAYOUT must be app or standalone)
endif
FW_CFLAGS := $(PROFILE_FLAGS) $(ARCH) -Os -g3 -std=gnu11 -ffreestanding -fno-builtin -fno-common -ffunction-sections -fdata-sections \
 -fstack-usage -Wall -Wextra -Wno-unused-parameter -MMD -MP -DCH32V30x_D8 -DHSE_VALUE=8000000 -DDMT_FIRMWARE=1 \
 '-DDMT_FW_VERSION="$(FW_VERSION)"' '-DDMT_FW_GIT="$(FW_GIT)"' $(FW_LAYOUT_DEFS) $(FW_DEFS) -Ifirmware $(INCLUDES)
FW_SDK_MODULES := gpio rcc usart dma spi exti tim adc misc flash
FW_C_SOURCES := $(MODULES) platform/wch_platform.c platform/wch_runtime.c platform/wch_nvic.c platform/memory.c \
 $(wildcard firmware/*.c) $(addprefix $(SDK)/Peripheral/src/ch32v30x_,$(addsuffix .c,$(FW_SDK_MODULES)))
FW_OBJECTS := $(addprefix $(FW_BUILD)/obj/,$(notdir $(FW_C_SOURCES:.c=.o))) $(FW_BUILD)/obj/startup.o
FW_ELF := $(FW_BUILD)/$(FW_NAME).elf
.PHONY: firmware firmware-clean restore-image
firmware: $(FW_ELF) $(FW_BUILD)/$(FW_NAME).bin $(FW_BUILD)/$(FW_NAME).hex $(FW_BUILD)/$(FW_NAME).lst $(FW_BUILD)/$(FW_NAME).size.txt
# Variants: SPI Flash read-only, and bring-up (read-only + verbose console).
.PHONY: firmware-readonly firmware-bringup firmware-all
firmware-readonly:
	$(MAKE) firmware FW_BUILD=$(BUILD)/firmware-readonly FW_NAME=dmt99-reconstructed-spi-readonly FW_DEFS=-DDMT_FW_SPI_FLASH_WRITES=0
firmware-bringup:
	$(MAKE) firmware FW_BUILD=$(BUILD)/firmware-bringup FW_NAME=dmt99-reconstructed-bringup 'FW_DEFS=-DDMT_FW_SPI_FLASH_WRITES=0 -DDMT_FW_BRINGUP=1'
# USB update images: the release (DMT_RELEASE_FEATURES: last measurement mode
# remembered, About shows POTAT-99 and the version) and a parity twin without them, which behaves exactly as V2.4 and is
# what the whole-image differentials compare.
.PHONY: firmware-update firmware-update-parity release
firmware-update:
	$(MAKE) firmware FW_LAYOUT=app FW_BUILD=$(BUILD)/firmware-update FW_NAME=dmt99-update FW_DEFS=-DDMT_RELEASE_FEATURES=1
firmware-update-parity:
	$(MAKE) firmware FW_LAYOUT=app FW_BUILD=$(BUILD)/firmware-update-parity FW_NAME=dmt99-update-parity
firmware-all: firmware firmware-readonly firmware-bringup firmware-update firmware-update-parity restore-image
# Copies the USB update image into final/v$(FW_VERSION)/ under the V2.4 file
# naming scheme, after checking its header, layout and version record.
release: firmware-update
	python3 ../tools/make_release.py --version $(FW_VERSION) --date $(FW_DATE) --git $(FW_GIT) \
	 --build $(BUILD)/firmware-update/dmt99-update --original $(ROOT)/DMT-99_V2.4_20240306.bin --out $(ROOT)/artifacts/firmware/potat-99
# Rollback image: 'j 0x6400' at 0x08000000 plus the unmodified V2.4 application.
restore-image: $(FW_BUILD)/dmt99-v2.4-restore.bin
$(FW_BUILD)/dmt99-v2.4-restore.bin: $(ROOT)/DMT-99_V2.4_20240306.bin ../tools/make_restore_image.py | $(FW_BUILD)/obj
	python3 ../tools/make_restore_image.py $< $@
$(FW_BUILD)/obj:
	mkdir -p $@
$(FW_BUILD)/compile_flags: FORCE | $(FW_BUILD)/obj
	@python3 ../tools/config_stamp.py $@ '$(CC_RV)' '$(FW_CFLAGS)'
define FW_C_RULE
$(FW_BUILD)/obj/$(notdir $(1:.c=.o)): $(1) $(FW_BUILD)/compile_flags | $(FW_BUILD)/obj
	$(CC_RV) $(FW_CFLAGS) -c $$< -o $$@
endef
$(foreach s,$(FW_C_SOURCES),$(eval $(call FW_C_RULE,$(s))))
$(FW_BUILD)/obj/startup.o: firmware/startup.S $(FW_BUILD)/compile_flags | $(FW_BUILD)/obj
	$(CC_RV) $(ARCH) -g $(FW_LAYOUT_DEFS) -c $< -o $@
$(FW_ELF): $(FW_OBJECTS) $(FW_LDSCRIPT) firmware/dmt99-sections.ld
	$(CC_RV) $(ARCH) -nostdlib -nostartfiles -Lfirmware -T$(FW_LDSCRIPT) -Wl,--gc-sections,--no-warn-rwx-segments,-Map=$(FW_BUILD)/$(FW_NAME).map,--print-memory-usage \
	 $(FW_OBJECTS) -lgcc -o $@
$(FW_BUILD)/$(FW_NAME).bin: $(FW_ELF)
	$(OBJCOPY) -O binary $< $@
$(FW_BUILD)/$(FW_NAME).hex: $(FW_ELF)
	$(OBJCOPY) -O ihex --change-addresses 0x08000000 $< $@
$(FW_BUILD)/$(FW_NAME).lst: $(FW_ELF)
	$(OBJDUMP) -d -S --no-show-raw-insn $< > $@
$(FW_BUILD)/$(FW_NAME).size.txt: $(FW_ELF) $(FW_BUILD)/$(FW_NAME).bin
	python3 ../tools/firmware_report.py $(FW_ELF) $(FW_BUILD)/$(FW_NAME).bin '$(SIZE)' '$(CC_RV)' > $@.tmp
	python3 ../tools/stack_report.py $(FW_ELF) $(OBJDUMP) $(FW_BUILD)/obj 6144 >> $@.tmp
	mv $@.tmp $@
	cat $@
firmware-clean:
	rm -rf $(FW_BUILD)
-include $(wildcard $(FW_BUILD)/obj/*.d)
