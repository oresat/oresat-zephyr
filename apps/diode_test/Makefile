APP= diode_test
BOARD= nucleo_f091rc
OPTIONS= -p always 
BOARD_PATH= "~/oresat/zephyr-workspace/oresat-zephyr/apps/diode_test/boards"
FLASH0_OVERLAY= "$(BOARD_PATH)/flash0.overlay"
SLOT0_OVERLAY=  "$(BOARD_PATH)/slot0.overlay"
BOARD_OVERLAY=  "$(BOARD_PATH)/$(BOARD).overlay"
RUNNER= openocd
BUILD_DIR= build/
SIGNED_BUILD_DIR= signed_build/

.PHONY: all nosysbuild write writenosigned canopen clean

all: clean
	@west build $(OPTIONS) -b $(BOARD) --sysbuild -d $(SIGNED_BUILD_DIR) . -- \
		-DDTC_OVERLAY_FILE="$(SLOT0_OVERLAY) $(FLASH0_OVERLAY) $(BOARD_OVERLAY)" \
		-DSB_CONFIG_BOOTLOADER_MCUBOOT=y \
		-Dmcuboot_EXTRA_DTC_OVERLAY_FILE="$(FLASH0_OVERLAY) $(BOARD_OVERLAY)"

nosysbuild: clean
	@west build $(OPTIONS) -b $(BOARD) .

write:
	@west flash -d $(SIGNED_BUILD_DIR) --runner $(RUNNER)

writenosigned:
	@west flash --runner $(RUNNER) 

canopen:
	@west flash -d $(SIGNED_BUILD_DIR) --domain $(APP) --runner canopen

clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(SIGNED_BUILD_DIR)
	@rm -rf gen
	@echo "cleaning build directories..."

