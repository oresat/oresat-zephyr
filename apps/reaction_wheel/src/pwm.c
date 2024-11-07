#include <stdint.h>
#include <assert.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include "stm32g4xx_hal_hrtim.h"
#include "hrtim.h"
#include "pwm.h"

/*
PHA: TIMER B: PA10 and PA11
PHB: TIMER E: PC8 and PC9
PHC: TIMER F: PC6 and PC7
*/

#define TIMERUPDATES (HRTIM_TIMERUPDATE_B | HRTIM_TIMERUPDATE_E | HRTIM_TIMERUPDATE_F)
#define OUTPUTS                                                                                    \
	(HRTIM_OUTPUT_TB1 | HRTIM_OUTPUT_TB2 | HRTIM_OUTPUT_TE1 | HRTIM_OUTPUT_TE2 |               \
	 HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2)
#define TIMERIDS (HRTIM_TIMERID_TIMER_B | HRTIM_TIMERID_TIMER_F | HRTIM_TIMERID_TIMER_E)

void hrtim_pwm_start()
{
	hrtim_pwm_set_dcycle_float(0.5f, 0.5f, 0.5f);

	__HAL_HRTIM_SETCOUNTER(&hhrtim1, HRTIM_TIMERINDEX_TIMER_B, 0);
	__HAL_HRTIM_SETCOUNTER(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, 0);
	__HAL_HRTIM_SETCOUNTER(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, 0);

	assert(HAL_HRTIM_SoftwareUpdate(&hhrtim1, TIMERUPDATES) == HAL_OK);

	assert(HAL_HRTIM_WaveformOutputStart(&hhrtim1, OUTPUTS) == HAL_OK);
	assert(HAL_HRTIM_WaveformCountStart_IT(&hhrtim1, TIMERIDS) == HAL_OK);

	assert(HAL_HRTIM_SoftwareUpdate(&hhrtim1, TIMERUPDATES) == HAL_OK);

	__HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_3,
			       PWM_CNTR_MAX_ALLOWED);
	__HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, HRTIM_COMPAREUNIT_3,
			       PWM_CNTR_MAX_ALLOWED);
	__HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, HRTIM_COMPAREUNIT_3,
			       PWM_CNTR_MAX_ALLOWED);

	HAL_HRTIM_SoftwareUpdate(&hhrtim1, TIMERUPDATES);
}

void hrtim_pwm_stop()
{
	assert(HAL_HRTIM_SoftwareUpdate(&hhrtim1, TIMERUPDATES) == HAL_OK);

	assert(HAL_HRTIM_WaveformCountStop(&hhrtim1, TIMERIDS) == HAL_OK);

	assert(HAL_HRTIM_SoftwareUpdate(&hhrtim1, TIMERUPDATES) == HAL_OK);

	assert(HAL_HRTIM_WaveformOutputStop(&hhrtim1, OUTPUTS) == HAL_OK);

	__HAL_HRTIM_SETCOUNTER(&hhrtim1, HRTIM_TIMERINDEX_TIMER_B, 0);
	__HAL_HRTIM_SETCOUNTER(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, 0);
	__HAL_HRTIM_SETCOUNTER(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, 0);

	HAL_HRTIM_WaveformSetOutputLevel(&hhrtim1, HRTIM_TIMERINDEX_TIMER_B, HRTIM_OUTPUT_TB1,
					 HRTIM_OUTPUTLEVEL_INACTIVE);
	HAL_HRTIM_WaveformSetOutputLevel(&hhrtim1, HRTIM_TIMERINDEX_TIMER_B, HRTIM_OUTPUT_TB2,
					 HRTIM_OUTPUTLEVEL_INACTIVE);
	HAL_HRTIM_WaveformSetOutputLevel(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, HRTIM_OUTPUT_TE1,
					 HRTIM_OUTPUTLEVEL_INACTIVE);
	HAL_HRTIM_WaveformSetOutputLevel(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, HRTIM_OUTPUT_TE2,
					 HRTIM_OUTPUTLEVEL_INACTIVE);
	HAL_HRTIM_WaveformSetOutputLevel(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, HRTIM_OUTPUT_TF1,
					 HRTIM_OUTPUTLEVEL_INACTIVE);
	HAL_HRTIM_WaveformSetOutputLevel(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, HRTIM_OUTPUT_TF2,
					 HRTIM_OUTPUTLEVEL_INACTIVE);

	hrtim_pwm_set_dcycle_float(0.5f, 0.5f, 0.5f);

	assert(HAL_HRTIM_SoftwareUpdate(&hhrtim1, TIMERUPDATES) == HAL_OK);
	assert(HAL_HRTIM_SoftwareUpdate(&hhrtim1, TIMERUPDATES) == HAL_OK);
	assert(HAL_HRTIM_SoftwareUpdate(&hhrtim1, TIMERUPDATES) == HAL_OK);
}

void hrtim_pwm_set_dcycle_uint16(uint16_t a_dcycle, uint16_t b_dcycle, uint16_t c_dcycle)
{
	a_dcycle = PWM_CNTR - a_dcycle;
	b_dcycle = PWM_CNTR - b_dcycle;
	c_dcycle = PWM_CNTR - c_dcycle;

	uint16_t a_compare = CLAMP(a_dcycle, PWM_CNTR_MIN_ALLOWED, PWM_CNTR_MAX_ALLOWED);
	uint16_t b_compare = CLAMP(b_dcycle, PWM_CNTR_MIN_ALLOWED, PWM_CNTR_MAX_ALLOWED);
	uint16_t c_compare = CLAMP(c_dcycle, PWM_CNTR_MIN_ALLOWED, PWM_CNTR_MAX_ALLOWED);
	__HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_B, HRTIM_COMPAREUNIT_1, a_compare);
	__HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_E, HRTIM_COMPAREUNIT_1, b_compare);
	__HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, HRTIM_COMPAREUNIT_1, c_compare);
}

void hrtim_pwm_set_dcycle_float(const float a_dcycle, const float b_dcycle, const float c_dcycle)
{
	hrtim_pwm_set_dcycle_uint16((uint16_t)(a_dcycle * ((float)PWM_CNTR)),
				    (uint16_t)(b_dcycle * ((float)PWM_CNTR)),
				    (uint16_t)(c_dcycle * ((float)PWM_CNTR)));
}
