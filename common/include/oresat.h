#ifndef _ORESAT_H_
#define _ORESAT_H_


#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/reboot.h>
#include <stdint.h>
#include <stdlib.h>
#include "worker.h"
//#include "can_hw.h"

#define CO_USE_GLOBALS
#define CO_DRIVER_CUSTOM
#define ORESAT_DEFAULT_ID 0x00
#define ORESAT_DEFAULT_BITRATE 1000

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
//    CANDriver *cand;
    uint8_t node_id;
    uint16_t bitrate;
//    const flt_reg_t *fifo1_filters;
    size_t filter_count;
} oresat_config_t;

/* OreSat initialization and main process */
int oresat_init(oresat_config_t *config);
int oresat_start(void);

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif
