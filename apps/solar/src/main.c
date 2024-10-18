#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/reboot.h>
#include <canopennode.h>
#include <OD.h>
#include "board_sensors.h"
#include "oresat.h"

#define CAN_INTERFACE DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus))
#define CAN_BITRATE                                                                                \
	(DT_PROP_OR(DT_CHOSEN(zephyr_canbus), bitrate,                                             \
		    DT_PROP_OR(DT_CHOSEN(zephyr_canbus), bus_speed, CONFIG_CAN_DEFAULT_BITRATE)) / \
	 1000)

int main(void)
{
	k_timepoint_t timepoint;
	uint8_t node_id = oresat_get_node_id();
	oresat_fix_cob_ids_in_od(node_id);

	canopennode_init(CAN_INTERFACE, CAN_BITRATE, node_id);
	board_sensors_init();

	while (canopennode_is_running()) {
		timepoint = sys_timepoint_calc(K_MSEC(1000));
		board_sensors_fill_od();
		k_sleep(sys_timepoint_timeout(timepoint));
	}

	canopennode_stop(CAN_INTERFACE);
	sys_reboot(SYS_REBOOT_COLD);

	return 0;
}
