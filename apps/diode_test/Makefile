#BOARD= diode_test
BOARD= nucleo_f091rc
OPTIONS= -p always 
BUILD_DIR= build/
EXTRA_OVERLAY= "/home/chroco/oresat/zephyr-workspace/oresat-zephyr/apps/diode_test/boards/nucleo_f091rc.overlay"
RUNNER= openocd
#RUNNER= stm32cubeprogrammer
SIGNED_BUILD_DIR= signed_build

.PHONY: all write clean

all: clean
	@west build $(OPTIONS) -b $(BOARD) --sysbuild -d $(SIGNED_BUILD_DIR) . -- -DSB_CONFIG_BOOTLOADER_MCUBOOT=y -Dmcuboot_EXTRA_DTC_OVERLAY_FILE=$(EXTRA_OVERLAY)

nosysbuild: clean
	@west build $(OPTIONS) -b $(BOARD) .

write:
	@west flash -d $(SIGNED_BUILD_DIR) --runner $(RUNNER)

nosigned_write:
	@west flash --runner $(RUNNER) 

clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(SIGNED_BUILD_DIR)
	@echo "cleaning build directories..."
