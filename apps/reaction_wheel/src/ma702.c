#include <stdbool.h>
#include <stdint.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>
#include "ma702.h"

#define MA702_REG_ZERO_POSITION        0
#define MA702_REG_BCT                  2
#define MA702_REG_TRIMMINGS            3
#define MA702_REG_ABZ                  4
#define MA702_REG_MAG_FIELD_THRESHOLDS 6
#define MA702_REG_ROTATION_DIRECTION   9
#define MA702_REG_MAG_FIELD_FLAGS      27

#define DEFAULT_MGL_THRESH_REG_VAL ((0b000 << 5) | (0b111 << 2))

// in mT (millitesla)
static const uint8_t field_strength_estimates[] = {
	0, 30, 45, 60, 74, 88, 102, 116,
};
#define FIELD_STRENGTH_ESTIMATES_COUNT ARRAY_SIZE(field_strength_estimates)

enum {
	MA702_ERROR_NONE = 0,
	MA702_ERROR_SPI_TRANSACTION,
	MA702_ERROR_WRITE_READBACK,
	MA702_ERROR_INVALID_ARGS,
} ma702_error_enum;

// spinlock to honor min reg read time
static inline void ma702_spinlock(void)
{
	for (uint8_t i = 0; i < 128; i++) {
		__asm__("nop");
	}
}

int ma702_zero(const struct spi_dt_spec *spec)
{
	uint16_t buffer = 0;
	const struct spi_buf buf = {
		.buf = &buffer,
		.len = sizeof(buffer),
	};
	const struct spi_buf_set buf_set = {
		.buffers = &buf,
		.count = 1,
	};
	return spi_transceive_dt(spec, &buf_set, &buf_set);
}

static bool ma702_write_reg(const struct spi_dt_spec *spec, uint8_t reg, uint8_t value)
{
	uint16_t tx_buffer = (((uint16_t)((0b100 << 5) | reg)) << 8) | value;
	const struct spi_buf tx_buf = {
		.buf = &tx_buffer,
		.len = sizeof(tx_buffer),
	};
	const struct spi_buf_set tx_buf_set = {
		.buffers = &tx_buf,
		.count = 1,
	};

	uint16_t rx_buffer = 0;
	const struct spi_buf rx_buf = {
		.buf = &rx_buffer,
		.len = sizeof(uint16_t),
	};
	const struct spi_buf_set rx_buf_set = {
		.buffers = &rx_buf,
		.count = 1,
	};

	spi_transceive_dt(spec, &tx_buf_set, &rx_buf_set);
	k_sleep(K_MSEC(20)); // delay for reg write to work

	// Reset to angle read
	tx_buffer = 0x0;
	rx_buffer = 0x0;

	int err = spi_transceive_dt(spec, &tx_buf_set, &rx_buf_set);
	if (err != 0) {
		return err;
	}
	ma702_spinlock();

	uint8_t value_readback = (uint8_t)(rx_buffer >> 8);
	return value_readback == value; // Confirm values written correctly
}

int ma702_read_reg(const struct spi_dt_spec *spec, uint8_t reg, uint16_t *value)
{
	uint16_t tx_buffer = ((uint16_t)((0b010 << 5) | 0x1B)) << 8;
	const struct spi_buf tx_buf = {
		.buf = &tx_buffer,
		.len = sizeof(uint16_t),
	};
	const struct spi_buf_set tx_buf_set = {
		.buffers = &tx_buf,
		.count = 1,
	};

	uint16_t rx_buffer = 0;
	const struct spi_buf rx_buf = {
		.buf = &rx_buffer,
		.len = sizeof(uint16_t),
	};
	const struct spi_buf_set rx_buf_set = {
		.buffers = &rx_buf,
		.count = 1,
	};

	int err = spi_transceive_dt(spec, &tx_buf_set, &rx_buf_set);
	if (err != 0) {
		return err;
	}
	ma702_spinlock();

	// Then read the contents, resetting tx_buf to 0 to queue an angle read next
	tx_buffer = 0x0;
	rx_buffer = 0x0;

	err = spi_transceive_dt(spec, &tx_buf_set, &rx_buf_set);
	if (err != 0) {
		return err;
	}
	ma702_spinlock();

	*value = rx_buffer;
	return 0;
}

int ma702_get_field_strength_flags(const struct spi_dt_spec *spec, ma702_field_enum *field_strength)
{
	uint16_t field_thresh;
	int err = ma702_read_reg(spec, MA702_REG_MAG_FIELD_FLAGS, &field_thresh);
	if (err != 0) {
		return err;
	}
	field_thresh >>= 8;

	bool mgh = (field_thresh & 0x80) == 0x80;
	// mgl value is not used
	bool mg1l = (field_thresh & 0x8) == 0x8;
	bool mg2l = (field_thresh & 0x4) == 0x4;
	bool mgl_corr = !(mg1l | mg2l);
	if (mgh && !mgl_corr) {
		*field_strength = MA702_FIELD_HIGH;
	} else if (!mgh && mgl_corr) {
		*field_strength = MA702_FIELD_LOW;
	} else if (!mgh && !mgl_corr) {
		*field_strength = MA702_FIELD_IN_RANGE;
	} else {
		*field_strength = MA702_FIELD_UNKNOWN;
	}
	return 0;
}

bool ma702_set_field_thresh(const struct spi_dt_spec *spec, uint8_t value)
{
	return ma702_write_reg(spec, MA702_REG_MAG_FIELD_THRESHOLDS, (uint16_t)value);
}

int ma702_get_field_strength_estimate_mt(const struct spi_dt_spec *spec, uint8_t *field_strength)
{
	ma702_field_enum field_strengths[FIELD_STRENGTH_ESTIMATES_COUNT] = {0};

	for (uint8_t i = 0; i < FIELD_STRENGTH_ESTIMATES_COUNT; i++) {
		// Set MGLT to i and MGHT to (7-i)
		uint8_t threshs = (i << 5) | (i << 2); //  low and high threshholds, both 3-bits
		if (!ma702_set_field_thresh(spec, threshs)) {
			goto write_fail;
		}

		ma702_get_field_strength_flags(spec, &field_strengths[i]);
		if (field_strengths[i] == MA702_FIELD_LOW) {
			return field_strength_estimates[i];
		}
	}

	// reset to default threshholds
	if (!ma702_set_field_thresh(spec, DEFAULT_MGL_THRESH_REG_VAL)) {
		goto write_fail;
	}

	return 255;

write_fail:
	return 0;
}

int ma702_get_angle_mdeg(const struct spi_dt_spec *spec, uint32_t *millidegrees)
{
	uint16_t angle;
	int ret = ma702_read_reg(spec, MA702_REG_ZERO_POSITION, &angle);
	if (ret == 0) {
		*millidegrees = ma702_angle_raw_to_mdeg(angle);
	}
	return ret;
}
