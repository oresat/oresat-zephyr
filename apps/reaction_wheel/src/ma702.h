#ifndef MA702_H
#define MA702_H

#include <stdint.h>
#include <zephyr/drivers/spi.h>

typedef enum {
	MA702_FIELD_HIGH,
	MA702_FIELD_LOW,
	MA702_FIELD_IN_RANGE,
	MA702_FIELD_UNKNOWN,
} ma702_field_enum;

typedef struct {
	bool x;
	bool y;
} ma702_trimmings_t;

typedef struct {
	uint8_t index_length_index_position: 4;
	uint8_t pluses_per_turn: 2;
} ma702_abz_t;

typedef struct {
	uint8_t mglt: 3;
	uint8_t mght: 3;
} ma702_mag_threadhold_t;

typedef struct {
	bool mgh;
	bool mgl;
	bool mg1l;
	bool mg2l;
} ma702_mag_flags_t;

typedef enum {
	MA702_CLOCKWISE = 0,
	MA702_COUNTER_CLOCKWISE = 1,
} ma702_rotation_direction_enum;

static inline uint32_t ma702_angle_raw_to_mdeg(uint16_t raw)
{
	return ((((raw - 0xFFFF) * -1) * 1000) / 0xFFFF) * 360;
}

static inline uint16_t ma702_angle_mdeg_to_raw(uint32_t millidegrees)
{
	return 0xFFFF - (uint16_t)(((millidegrees / 360) * 0xFFFF) / 1000);
}

int ma702_get_angle_mdeg(const struct spi_dt_spec *spec, uint32_t *millidegrees);
int ma702_get_bct(const struct spi_dt_spec *spec, uint8_t *bct);
int ma702_get_trimmings(const struct spi_dt_spec *spec, ma702_trimmings_t *trimmings);
int ma702_get_abz(const struct spi_dt_spec *spec, ma702_abz_t *abz);
int ma702_get_mag_thresholds(const struct spi_dt_spec *spec, ma702_mag_threadhold_t *threshholds);
int ma702_get_rotation_direction(const struct spi_dt_spec *spec,
				 ma702_rotation_direction_enum *direciton);
int ma702_get_mag_flags(const struct spi_dt_spec *spec, ma702_mag_flags_t *mag_flags);
int ma702_get_field_strength_estimate_mt(const struct spi_dt_spec *spec, uint8_t *milliteslas);

int ma702_set_zero_position(const struct spi_dt_spec *spec);
int ma702_set_mag_thresholds(const struct spi_dt_spec *spec, ma702_mag_threadhold_t threshholds);

#endif
