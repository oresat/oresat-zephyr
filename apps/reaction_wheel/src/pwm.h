#ifndef PWM_H
#define PWM_H

#define PWM_CNTR      24000
#define HRTIM_DLL_MUL 32

// 4x HRTIM clock freq, so that up/down set/reset can be changed
#define PWM_CNTR_MARGIN      (4 * (HRTIM_DLL_MUL))
#define PWM_CNTR_MAX_ALLOWED (PWM_CNTR - PWM_CNTR_MARGIN)
#define PWM_CNTR_MIN_ALLOWED (PWM_CNTR_MARGIN)
#define PWM_FREQ             192000
#define PWM_RCR              6

void hrtim_pwm_start(void);
void hrtim_pwm_stop(void);
void hrtim_pwm_set_dcycle_float(float a_dcycle, float b_dcycle, float c_dcycle);

#endif
