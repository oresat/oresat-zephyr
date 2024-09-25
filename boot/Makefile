# Description: 
# ---------------------------------------------------------------------------------
# Makefile to build mcuboot and flash to target board, zephyr's west tool is used. 
# This implementation is for the nucleo_f091rc dev board. TODO: Future implementation
# should work for multiple boards. 
#
# [IMPORTANT] Ensure the output of the flash runner shows that the MCUBoot application 
# runs from 0x0800_0000, otherwise application will note work. Additionally, from 
# Zephyr's perspective mcuboot is an application, hence, building and flashing is the exact 
# same as in application development. 
# 
# Reference Material: 
# ---------------------------------------------------------------------------------
# MCUBoot comes packages with an example application that this is losely based on
# see `zephyrproject/bootloader/mcuboot/samples/zephyr/Makefile` for more detail. 
#
# Running: 
# 	| $ make <BOARD=optional> bootloader
# 	|
# 	|-> ### pass argument board to adjust hard-coded target below
#
# ---------------------------------------------------------------------------------

# Automatically detect the serial port
SERIAL_PORT := $(shell ls /dev/ttyACM* 2>/dev/null | head -n 1)
ifeq ($(SERIAL_PORT),)
SERIAL_PORT := $(shell ls /dev/ttyUSB* 2>/dev/null | head -n 1)
endif

BOARD ?= nucleo_f091rc

SOURCE_DIR := $(CURDIR)
BUILD_DIR_BOOT := $(CURDIR)/build/$(BOARD)
MCUBOOT_SRC_DIR := $(ZEPHYR_BASE)/../bootloader/mcuboot/boot/zephyr
OVERLAY_CONFIG := $(SOURCE_DIR)/boards/boot-stm32f091rc.overlay
PRJ_CONF := $(SOURCE_DIR)/prj.conf

.PHONY: check bootloader clean monitor

bootloader: check 
	@echo "Build MCUBoot for $(BOARD)..."
	west build -b $(BOARD) -d $(BUILD_DIR_BOOT) $(MCUBOOT_SRC_DIR) \
		-p -- -DDTC_OVERLAY_FILE=$(OVERLAY_CONFIG) \
		-DCONF_FILE=$(PRJ_CONF)
	@echo "MCUboot binary is at: $(BUILD_DIR_BOOT)/zephyr/zephyr.bin"

flash: bootloader
	@echo "Flashing MCUboot to $(BOARD)..."
	west flash -d $(BUILD_DIR_BOOT)

clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR_BOOT)

monitor: 
	@echo "Monitoring serial output... Be sure you've flashed MCUboot before running this."
	@if [ -z "$(SERIAL_PORT)" ]; then \
		echo "Error: No serial port found. Make sure your device is connected."; \
		exit 1; \
	fi
	@echo "Using serial port: $(SERIAL_PORT)"
	@echo "Press Ctrl+A, K, Y to exit the screen session."
	@sleep 2  # Give user time to read the instructions
	@screen $(SERIAL_PORT) 115200

# You can add more targets as needed, e.g., for debugging
debug: bootloader
	@echo "Starting debug session..."
	west debug -d $(BUILD_DIR_BOOT)

check:
	@if [ -z "$$ZEPHYR_BASE" ]; then echo "Zephyr environment not set up"; false; fi
	@if [ -z "$(BOARD)" ]; then echo "You must specify BOARD=<board>"; false; fi

