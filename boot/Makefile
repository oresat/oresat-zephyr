# Description: 
# --------------------------------------------------------------------------------
# Makefile to build mcuboot and flash to target board, zephyr's west tool is used. 
# This implementation is for the nucleo_f091rc dev board but other targets should work
# as well. 
#
# [IMPORTANT] Ensure the output of the flash runner shows that the MCUBoot application 
# runs from 0x0800_0000, otherwise application will not work. Additionally, the application
# itself should start at the offset specified in your overlay file. See 
# boot/app-stm32f091rc.overlay files. The zephyr,code-partition property in chosen/ 
# should properly set; then enable the Kconfig variable CONFIG_USE_DT_CODE_PARTITION=y
# so your application gets linked to the correct starting address in flash storage. 
# 
# Author: Reece Wayt
#
# Reference Material: 
# ---------------------------------------------------------------------------------
# MCUBoot comes packaged with an example application that this Makefile is based on
# see `zephyrproject/bootloader/mcuboot/samples/zephyr/Makefile` for more detail. 
#
# Usage: 
#
# 	| $ make <BOARD=optional> <target>
# 	|
# 	|-> ### pass argument board to adjust hard-coded target below
# TODO: 	
# ---------------------------------------------------------------------------------- 	
# MCUboot comes packaged with some keys, this makefile uses said key; This should not
# be used in production code. Use the siging tool instead.
#
# Example Usage of signing tool: 
# 	$ west sign -t imgtool -- --key bootloader/mcuboot/root-rsa-2048.pem
#	
# The above command creates an image file called zephyr.signed.bin in the build directory.
# -----------------------------------------------------------------------------------

# Automatically detect the serial port
SERIAL_PORT := $(shell ls /dev/ttyACM* 2>/dev/null | head -n 1)
ifeq ($(SERIAL_PORT),)
SERIAL_PORT := $(shell ls /dev/ttyUSB* 2>/dev/null | head -n 1)
endif

# Configuration
BOARD ?= nucleo_f091rc
SOURCE_DIR := $(CURDIR)
BUILD_DIR_BOOT := $(CURDIR)/build/bootloader
BUILD_DIR_APP := $(CURDIR)/build/app
MCUBOOT_SRC_DIR := $(ZEPHYR_BASE)/../bootloader/mcuboot/boot/zephyr
TEST_BLINKY_APP_SRC_DIR := $(ZEPHYR_BASE)/samples/basic/blinky
BOOT_OVERLAY_CONFIG := $(SOURCE_DIR)/boards/boot-stm32f091rc.overlay
APP_OVERLAY_CONFIG := $(SOURCE_DIR)/boards/app-stm32f091rc.overlay
BOOT_CONF := $(SOURCE_DIR)/boot.conf
APP_CONF := $(SOURCE_DIR)/app.conf
IMGTOOL = $(ZEPHYR_BASE)/../bootloader/mcuboot/scripts/imgtool.py
SIGNING_KEY ?= $(ZEPHYR_BASE)/../bootloader/mcuboot/root-rsa-2048.pem

# Phony targets
.PHONY: all check bootloader app clean monitor debug flash_app flash_boot

# Default target
all: check bootloader app flash_boot flash_app

# Check environment
check:
	@if [ -z "$$ZEPHYR_BASE" ]; then echo "Error: Zephyr environment not set up"; exit 1; fi
	@if [ -z "$(BOARD)" ]; then echo "Error: You must specify BOARD=<board>"; exit 1; fi
	@if [ ! -f "$(SIGNING_KEY)" ]; then echo "Error: Signing key not found at $(SIGNING_KEY)"; exit 1; fi
	@if [ ! -d "$(MCUBOOT_SRC_DIR)" ]; then echo "Error: MCUboot source directory not found"; exit 1; fi
	@if [ ! -d "$(TEST_BLINKY_APP_SRC_DIR)" ]; then echo "Error: Blinky app source directory not found"; exit 1; fi
	@echo "Environment check passed."

# Build MCUboot
bootloader: check
	@echo "Building MCUBoot for $(BOARD)..."
	west build -b $(BOARD) -d $(BUILD_DIR_BOOT) $(MCUBOOT_SRC_DIR) \
		-p -- -DDTC_OVERLAY_FILE=$(BOOT_OVERLAY_CONFIG) \
		-DCONF_FILE=$(BOOT_CONF)
	@if [ ! -f "$(BUILD_DIR_BOOT)/zephyr/zephyr.bin" ]; then echo "Error: MCUboot binary not generated"; exit 1; fi
	@echo "MCUboot binary is at: $(BUILD_DIR_BOOT)/zephyr/zephyr.bin"

# Build application
app: check
	@echo "Building Test Application for MCUBoot, target $(BOARD)..."
	west build -b $(BOARD) -d $(BUILD_DIR_APP) $(TEST_BLINKY_APP_SRC_DIR) \
		-p -- -DDTC_OVERLAY_FILE=$(APP_OVERLAY_CONFIG) \
		-DCONF_FILE=$(APP_CONF) 
	@if [ ! -f "$(BUILD_DIR_APP)/zephyr/zephyr.bin" ]; then echo "Error: Application binary not generated"; exit 1; fi
	@echo "Application binary is at: $(BUILD_DIR_APP)/zephyr/zephyr.bin"
	@echo "Using West sign to sign application with $(SIGNING_KEY)"
	west sign -d $(BUILD_DIR_APP) -t imgtool -- --key $(SIGNING_KEY)
	@echo "Application signing complete."

# Flash application
flash_app: app
	@echo "Flashing application to board..."
	west flash -d $(BUILD_DIR_APP)

# Flash MCUboot
flash_boot: bootloader
	@echo "Flashing MCUboot to $(BOARD)..."
	west flash -d $(BUILD_DIR_BOOT)

# Clean build directories
clean:
	@echo "Cleaning build directories..."
	rm -rf $(BUILD_DIR_APP)
	rm -rf $(BUILD_DIR_BOOT)
	@echo "Clean complete."

# Monitor serial output
monitor: 
	@echo "Monitoring serial output..."
	@if [ -z "$(SERIAL_PORT)" ]; then \
		echo "Error: No serial port found. Make sure your device is connected."; \
		exit 1; \
	fi
	@echo "Using serial port: $(SERIAL_PORT)"
	@echo "Press Ctrl+A, K, Y to exit the screen session."
	@sleep 2  # Give user time to read the instructions
	screen $(SERIAL_PORT) 115200


# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build and flash both MCUboot and the application"
	@echo "  check        - Check the environment setup"
	@echo "  bootloader   - Build MCUboot"
	@echo "  app          - Build and sign the application"
	@echo "  flash_boot   - Flash MCUboot to the board"
	@echo "  flash_app    - Flash the application to the board"
	@echo "  clean        - Clean build directories"
	@echo "  monitor      - Monitor serial output"
	@echo "  help         - Show this help message"-
