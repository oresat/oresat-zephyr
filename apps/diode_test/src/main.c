#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/reboot.h>
#include <canopennode.h>
#include <OD.h>
#include "board_sensors.h"

#define CAN_INTERFACE DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus))
#define CAN_BITRATE (DT_PROP_OR(DT_CHOSEN(zephyr_canbus), bitrate, \
					  DT_PROP_OR(DT_CHOSEN(zephyr_canbus), bus_speed, \
						     CONFIG_CAN_DEFAULT_BITRATE)) / 1000)

int main(void) {
	canopennode_init(CAN_INTERFACE, CAN_BITRATE, 0x54);
	board_sensors_init();

	while (canopennode_is_running()) {
		k_sleep(K_MSEC(1000));
		board_sensors_fill_od();
	}

	canopennode_stop(CAN_INTERFACE);
	sys_reboot(SYS_REBOOT_COLD);
	return 0;
}
