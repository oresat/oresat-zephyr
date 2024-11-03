#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/reboot.h>
#include <canopennode.h>
#include <OD.h>
#include "board_sensors.h"
#include "oresat.h"
#include "aux_adc.h"

#define RW_BASE_NODE_ID 0x3C
#define CAN_INTERFACE   DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus))
#define CAN_BITRATE                                                                                \
	(DT_PROP_OR(DT_CHOSEN(zephyr_canbus), bitrate,                                             \
		    DT_PROP_OR(DT_CHOSEN(zephyr_canbus), bus_speed, CONFIG_CAN_DEFAULT_BITRATE)) / \
	 1000)

static const struct gpio_dt_spec debug0 = GPIO_DT_SPEC_GET(DT_NODELABEL(debug0), gpios);
static const struct gpio_dt_spec debug1 = GPIO_DT_SPEC_GET(DT_NODELABEL(debug1), gpios);
static const struct gpio_dt_spec debug2 = GPIO_DT_SPEC_GET(DT_NODELABEL(debug2), gpios);
static const struct gpio_dt_spec debug3 = GPIO_DT_SPEC_GET(DT_NODELABEL(debug3), gpios);

uint8_t rw_get_node_id(void)
{
	int ret;

	if (!gpio_is_ready_dt(&debug0) || !gpio_is_ready_dt(&debug1) ||
	    !gpio_is_ready_dt(&debug2) || !gpio_is_ready_dt(&debug3)) {
		return DEFAULT_NODE_ID;
	}

	ret = gpio_pin_configure_dt(&debug0, GPIO_OUTPUT);
	ret += gpio_pin_configure_dt(&debug1, GPIO_INPUT);
	ret += gpio_pin_configure_dt(&debug2, GPIO_INPUT);
	ret += gpio_pin_configure_dt(&debug3, GPIO_INPUT);
	if (ret < 0) {
		return DEFAULT_NODE_ID;
	}

	gpio_pin_set_dt(&debug0, 1);
	int debug1_val = gpio_pin_get_dt(&debug1);
	int debug2_val = gpio_pin_get_dt(&debug2);
	int debug3_val = gpio_pin_get_dt(&debug3);
	gpio_pin_set_dt(&debug0, 0);

	if ((debug1_val < 0) || (debug2_val < 0) || (debug3_val < 0)) {
		return DEFAULT_NODE_ID;
	}
	return RW_BASE_NODE_ID + ((debug1_val + debug2_val + debug3_val) * 4);
}

int main(void)
{
	k_timepoint_t timepoint;
	uint8_t node_id = rw_get_node_id();

	canopennode_init(CAN_INTERFACE, CAN_BITRATE, node_id);
	board_sensors_init();
	aux_adc_init();

	while (canopennode_is_running()) {
		timepoint = sys_timepoint_calc(K_MSEC(1000));
		board_sensors_fill_od();
		aux_adc_update();
		k_sleep(sys_timepoint_timeout(timepoint));
	}

	canopennode_stop(CAN_INTERFACE);
	sys_reboot(SYS_REBOOT_COLD);

	return 0;
}
