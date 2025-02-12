APP= canopennode
BOARD= nucleo_f091rc
OPTIONS= -p always 
RUNNER= openocd
SRC= $(ZEPHYR_BASE)/samples/modules/canopennode
ZEPHYR_WORKSPACE= /home/chroco/oresat/zephyr-workspace
APP_ROOT= $(ZEPHYR_WORKSPACE)/oresat-zephyr/apps/$(APP)
BOARD_DIR= $(APP_ROOT)/boards
FLASH0_OVERLAY= $(BOARD_DIR)/flash0.overlay
APP_OVERLAY= $(BOARD_DIR)/app.overlay
APP_CONF= $(APP_ROOT)/app.conf
BOARD_OVERLAY= $(BOARD_DIR)/$(BOARD).overlay
BUILD_DIR= build/
SIGNED_BUILD_DIR= signed_build/

.PHONY: sysbuild write nosysbuild nosigned_write canopen confirm menuconfig debug clean

sysbuild: clean
	@west build $(OPTIONS) -b $(BOARD) \
		--sysbuild -d $(SIGNED_BUILD_DIR) $(SRC) -- \
		-DDTC_OVERLAY_FILE="$(APP_OVERLAY) $(FLASH0_OVERLAY)"  \
		-Dcanopennode_CONF_FILE="prj_img_mgmt.conf $(APP_CONF)" \
		-DSB_CONFIG_BOOTLOADER_MCUBOOT=y \
		-Dmcuboot_EXTRA_DTC_OVERLAY_FILE="$(FLASH0_OVERLAY)" \
		-Dmcuboot_OVERLAY_CONFIG="$(APP_CONF)" 

write:
	@west flash -d $(SIGNED_BUILD_DIR) --runner $(RUNNER)

nosysbuild: clean
	@west build $(OPTIONS) -b $(BOARD) $(SRC) .

nosigned_write:
	@west flash --skip-rebuild --runner $(RUNNER) 

canopen:
	@west flash -d $(SIGNED_BUILD_DIR) --skip-rebuild --domain $(APP) --runner canopen --timeout 20

confirm:
	@west flash -d $(SIGNED_BUILD_DIR) --skip-rebuild --domain $(APP) --runner canopen --confirm-only

menuconfig:
	@west build -t menuconfig -b $(BOARD) $(SRC)

debug:
	@west debug -b $(BOARD) $(SRC)

clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(SIGNED_BUILD_DIR)
	@echo "cleaning build directories..."

