#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include "aux_adc.h"

static const struct adc_dt_spec adc_channels[] = {
	ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), therm_a),
	ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), therm_b),
};
#define CHANNEL_COUNT ARRAY_SIZE(adc_channels)

static uint16_t buf = 0;
static struct adc_sequence sequence = {
	.buffer = &buf,
	/* buffer size in bytes, not number of samples */
	.buffer_size = sizeof(buf),
};

void aux_adc_init(void)
{
	int err;

	/* Configure channels individually prior to sampling. */
	for (size_t i = 0U; i < CHANNEL_COUNT; i++) {
		if (!adc_is_ready_dt(&adc_channels[i])) {
			printk("ADC controller device %s not ready\n", adc_channels[i].dev->name);
			return;
		}

		err = adc_channel_setup_dt(&adc_channels[i]);
		if (err < 0) {
			printk("Could not setup channel #%d (%d)\n", i, err);
			return;
		}
	}
}

void aux_adc_update(void)
{
	int err;

	for (size_t i = 0U; i < CHANNEL_COUNT; i++) {
		int32_t val_mv;

		printk("- %s, channel %d: ", adc_channels[i].dev->name, adc_channels[i].channel_id);

		(void)adc_sequence_init_dt(&adc_channels[i], &sequence);

		err = adc_read_dt(&adc_channels[i], &sequence);
		if (err < 0) {
			printk("Could not read (%d)\n", err);
			continue;
		}

		/*
		 * If using differential mode, the 16 bit value
		 * in the ADC sample buffer should be a signed 2's
		 * complement value.
		 */
		if (adc_channels[i].channel_cfg.differential) {
			val_mv = (int32_t)((int16_t)buf);
		} else {
			val_mv = (int32_t)buf;
		}
		printk("%" PRId32, val_mv);
		err = adc_raw_to_millivolts_dt(&adc_channels[i], &val_mv);
		/* conversion to mV may not be supported, skip if not */
		if (err < 0) {
			printk(" (value in mV not available)\n");
		} else {
			printk(" = %" PRId32 " mV\n", val_mv);
		}
	}
}
