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
#define LED0_NODE DT_ALIAS(led0)

int main(void) {
	const struct device *const dev_gyro = DEVICE_DT_GET_ONE(bosch_bmi08x_gyro);
	static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
	struct sensor_value gyr[3];
	struct sensor_value full_scale;
	struct sensor_value sampling_freq;
	struct sensor_value oversampling;
	bool gyro_ok = false;

	canopennode_init(CAN_INTERFACE, CAN_BITRATE, 0x38);

	board_sensors_init();

	if (!device_is_ready(dev_gyro)) {
		printf("Device %s is not ready\n", dev_gyro->name);
	} else {
		gyro_ok = true;
	}

	if (!gpio_is_ready_dt(&led)) {
		printf("LED is not ready\n");
	}

	int ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		printf("LED failed to be configured\n");
	}

	printf("Device %p name is %s\n", dev_gyro, dev_gyro->name);

	/* Setting scale in degrees/s to match the sensor scale */
	full_scale.val1 = 500;          /* dps */
	full_scale.val2 = 0;
	sampling_freq.val1 = 100;       /* Hz. Performance mode */
	sampling_freq.val2 = 0;
	oversampling.val1 = 1;          /* Normal mode */
	oversampling.val2 = 0;

	if (gyro_ok) {
		sensor_attr_set(dev_gyro, SENSOR_CHAN_GYRO_XYZ, SENSOR_ATTR_FULL_SCALE, &full_scale);
		sensor_attr_set(dev_gyro, SENSOR_CHAN_GYRO_XYZ, SENSOR_ATTR_OVERSAMPLING, &oversampling);
		/* Set sampling frequency last as this also sets the appropriate
		 * power mode. If already sampling, change sampling frequency to
		 * 0.0Hz before changing other attributes
		 */
		sensor_attr_set(dev_gyro, SENSOR_CHAN_GYRO_XYZ, SENSOR_ATTR_SAMPLING_FREQUENCY, &sampling_freq);
	}

	while (canopennode_is_running()) {
		k_sleep(K_MSEC(1000));

		gpio_pin_toggle_dt(&led);

		board_sensors_fill_od();

		if (gyro_ok) {
			sensor_sample_fetch(dev_gyro);
			sensor_channel_get(dev_gyro, SENSOR_CHAN_GYRO_XYZ, gyr);

			CO_LOCK_OD(CO->CANmodule);
			OD_RAM.x4000_gyroscope.pitch_rate = gyr[0].val1;
			OD_RAM.x4000_gyroscope.yaw_rate = gyr[1].val1;
			OD_RAM.x4000_gyroscope.roll_rate = gyr[2].val1;
			CO_UNLOCK_OD(CO->CANmodule);

			printf("GX: %d.%06d; GY: %d.%06d; GZ: %d.%06d;\n",
						 gyr[0].val1, gyr[0].val2,
						 gyr[1].val1, gyr[1].val2,
						 gyr[2].val1, gyr[2].val2);
		}
	}

	canopennode_stop(CAN_INTERFACE);
	sys_reboot(SYS_REBOOT_COLD);
	return 0;
}
