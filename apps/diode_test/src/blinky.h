#ifndef BLINKY_H
#define BLINKY_H

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define BLINKY_STACK_SIZE 500
#define BLINKY_PRIORITY 5

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

#define LED0_NODE DT_ALIAS(led0)

extern void blinky_entry(void *, void *, void *);
extern int startBlinkyThread(void);

#endif // BLINKY_H


