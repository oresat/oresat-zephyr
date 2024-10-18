#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <canopennode.h>
#include <OD.h>
#include "oresat.h"

static const struct device *const dev_gyro = DEVICE_DT_GET_ONE(bosch_bmi08x_gyro);
static bool gyro_ok = false;

void imu_init(void)
{
	struct sensor_value full_scale;
	struct sensor_value sampling_freq;
	struct sensor_value oversampling;

	gyro_ok = device_is_ready(dev_gyro);
	if (!gyro_ok) {
		printf("Device %s is not ready\n", dev_gyro->name);
	} else {
		/* Setting scale in degrees/s to match the sensor scale */
		full_scale.val1 = 500; /* dps */
		full_scale.val2 = 0;
		sampling_freq.val1 = 100; /* Hz. Performance mode */
		sampling_freq.val2 = 0;
		oversampling.val1 = 1; /* Normal mode */
		oversampling.val2 = 0;

		sensor_attr_set(dev_gyro, SENSOR_CHAN_GYRO_XYZ, SENSOR_ATTR_FULL_SCALE,
				&full_scale);
		sensor_attr_set(dev_gyro, SENSOR_CHAN_GYRO_XYZ, SENSOR_ATTR_OVERSAMPLING,
				&oversampling);
		sensor_attr_set(dev_gyro, SENSOR_CHAN_GYRO_XYZ, SENSOR_ATTR_SAMPLING_FREQUENCY,
				&sampling_freq);
	}
}

void imu_fill_od(void)
{
	struct sensor_value gyr[3];
	if (gyro_ok) {
		sensor_sample_fetch(dev_gyro);
		sensor_channel_get(dev_gyro, SENSOR_CHAN_GYRO_XYZ, gyr);

		CO_LOCK_OD(CO->CANmodule);
		OD_RAM.x4000_gyroscope.pitch_rate = sensor_value_to_milli(&gyr[0]);
		OD_RAM.x4000_gyroscope.yaw_rate = sensor_value_to_milli(&gyr[1]);
		OD_RAM.x4000_gyroscope.roll_rate = sensor_value_to_milli(&gyr[2]);
		CO_UNLOCK_OD(CO->CANmodule);

		printf("GX: %d.%06d; GY: %d.%06d; GZ: %d.%06d;\n", gyr[0].val1, gyr[0].val2,
		       gyr[1].val1, gyr[1].val2, gyr[2].val1, gyr[2].val2);
	}
}
