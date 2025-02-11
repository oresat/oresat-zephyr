APP= diode_test
BOARD= nucleo_f091rc
OPTIONS= -p always 
BUILD_DIR= build/
FLASH0_OVERLAY= "/home/chroco/oresat/zephyr-workspace/oresat-zephyr/apps/diode_test/boards/flash0.overlay"
MCUBOOT_OVERLAY= "/home/chroco/oresat/zephyr-workspace/oresat-zephyr/apps/diode_test/boards/mcuboot.overlay"
RUNNER= openocd
#RUNNER= stm32cubeprogrammer
SIGNED_BUILD_DIR= signed_build

.PHONY: all nosysbuild write nosigned_write canopen clean

all: clean
	@west build $(OPTIONS) -b $(BOARD) \
		--sysbuild -d $(SIGNED_BUILD_DIR) . -- \
		-DDTC_OVERLAY_FILE="$(MCUBOOT_OVERLAY) $(FLASH0_OVERLAY)" \
#		-DDTC_OVERLAY_FILE=$(FLASH0_OVERLAY) \
		-DSB_CONFIG_BOOTLOADER_MCUBOOT=y -Dmcuboot_EXTRA_DTC_OVERLAY_FILE=$(FLASH0_OVERLAY)

nosysbuild: clean
	@west build $(OPTIONS) -b $(BOARD) .

write:
	@west flash -d $(SIGNED_BUILD_DIR) --runner $(RUNNER)

nosigned_write:
	@west flash --runner $(RUNNER) 

canopen:
	@west flash -d $(SIGNED_BUILD_DIR) --domain $(APP) --runner canopen

clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(SIGNED_BUILD_DIR)
#	@rm -rf gen
	@echo "cleaning build directories..."
