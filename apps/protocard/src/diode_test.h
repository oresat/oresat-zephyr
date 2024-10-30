#ifndef _DIODE_TEST_H_
#define _DIODE_TEST_H_

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "stdint.h"

#define ENABLE_NV_MEMORY_UPDATE_CODE      0
#define ENABLE_SERIAL_DEBUG_OUTPUT        1

#define LED0_NODE DT_ALIAS(led0)
#define MUX_EN_NODE DT_ALIAS(mux_en)

/*
#if ENABLE_SERIAL_DEBUG_OUTPUT || ENABLE_NV_MEMORY_UPDATE_CODE
#define DEBUG_SERIAL    (BaseSequentialStream*) &SD2

#define dbgprintf(str, ...) chprintf((BaseSequentialStream*) &SD2, str, ##__VA_ARGS__)
#else
#define dbgprintf(str, ...)
#endif
//*/

typedef uint16_t adcsample_t;

#define DTC_NODE_ID             0x54

// definitions from the board file
#define DTC_LED                 GPIOA_LED
#define DTC_MUX_EN              GPIOB_LED_MUX_EN
#define DTC_MUX_A0              GPIOB_LED_MUX_A0
#define DTC_MUX_A1              GPIOB_LED_MUX_A1
#define DTC_MUX_A2              GPIOB_LED_MUX_A2
#define DTC_POUT                GPIOB_POUTPROT  //PB0

#define DTC_NUM_DIODES          8

#define SAMPLES                 2  // number of samples in sample buffer

// bits for OD error object
#define ERROR_DAC               (1 << 0)
#define ERROR_ADC_CB            (1 << 1)
#define ERROR_ADC_START         (1 << 2)
#define ERROR_ADC_STOP          (1 << 3)
#define ERROR_4                 (1 << 4)
#define ERROR_5                 (1 << 5)

// bits for OD status object
#define CTRL_DAC_EN             0x0
#define CTRL_GPT_EN             0x1
#define CTRL_ADC_EN             0x2
#define CTRL_MUX_EN             0x3
#define CTRL_MUX_A0             0x4
#define CTRL_MUX_A1             0x5
#define CTRL_MUX_A2             0x6
#define CTRL_7                  0x7
#define CTRL_8                  0x8
#define CTRL_9                  0x9
#define CTRL_A                  0xA
#define CTRL_B                  0xB
#define CTRL_C                  0xC
#define CTRL_D                  0xD
#define CTRL_E                  0xE
#define CTRL_F                  0xF

// bit masks
#define CTRL_MUX_MASK           (0x7 << CTRL_MUX_A0)

/*
 * sample_t struct for buffering samples
 */
typedef struct
{
  adcsample_t led_current;          
  adcsample_t led_swir_pd_current;  
  adcsample_t uv_pd_current;        
  adcsample_t tsen;                 
} sample_t;

#define NUM_CHANNELS            sizeof(sample_t)/sizeof(adcsample_t)

#define MAX_FUNCTIONS           64

/*
 * DTC struct
 */
typedef struct
{
	// pointers to OD objects
  uint8_t *pctrl;
  uint8_t *pmux_select;
  uint16_t *pdac;
  uint16_t *pstatus;
  uint16_t *perror;
  void (*pfunc[MAX_FUNCTIONS])(void);
  adcsample_t *pled_current;
  adcsample_t *pled_swir_pd_current;
  adcsample_t *puv_pd_current;
  adcsample_t *ptsen;
  adcsample_t *padcsample;
  int lastFunctionIndex; // last index of functions set in *pfunc[]
} DTC;

/*
 * thread section 
 */

#define BLINKY_STACK_SIZE 500
#define BLINKY_PRIORITY 5

#define WATCH_STACK_SIZE 500
#define WATCH_PRIORITY 5

#define CONTROL_STACK_SIZE 500
#define CONTROL_PRIORITY 5

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

extern int startBlinkyThread(void);
extern int startWatchThread(void);
extern int startControlThread(void);

/*
 * function declaration
 */
void dtc_init(void);
void dtc_callCtrlThreadFunctions(void);
void dtc_dacStart(void);
void dtc_dacStop(void);
void dtc_dacSet(void);
void dtc_gptStart(void);
void dtc_gptStop(void);
void dtc_adcStart(void);
void dtc_adcStop(void);
void dtc_muxEnable(void);
void dtc_muxDisable(void);
void dtc_muxSelect(void);
void dtc_clearErrors(void);

#endif // _DIODE_TEST_H_
