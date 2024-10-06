#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>
#include <zephyr/device.h>
#include <zephyr/drivers/can.h>
#include <CANopen.h>
#include <OD.h>
#include <canopennode.h>

#define CO_MAIN_STACK_SIZE 2048
#define CO_RT_STACK_SIZE 2048
#define CO_MAIN_PRIORITY 5
#define CO_RT_PRIORITY 5

/* defaults values for CO_CANopenInit() */
#define NMT_CONTROL \
	(CO_NMT_STARTUP_TO_OPERATIONAL | CO_NMT_ERR_ON_ERR_REG | CO_ERR_REG_GENERIC_ERR | CO_ERR_REG_COMMUNICATION)
#define FIRST_HB_TIME 500
#define SDO_SRV_TIMEOUT_TIME 1000
#define SDO_CLI_TIMEOUT_TIME 1000
#define SDO_CLI_BLOCK false
#define OD_STATUS_BITS NULL

CO_t *CO = NULL;
static CO_NMT_reset_cmd_t reset = CO_RESET_NOT;

K_THREAD_STACK_DEFINE(co_main_stack_area, CO_MAIN_STACK_SIZE);
K_THREAD_STACK_DEFINE(co_rt_stack_area, CO_RT_STACK_SIZE);
struct k_thread co_main_thread_data;
struct k_thread co_rt_thread_data;
k_tid_t co_main_tid;
k_tid_t co_rt_tid;

static void co_main_thread(void *p1, void *p2, void *p3);
static void co_rt_thread(void *p1, void *p2, void *p3);

int canopennode_init(const struct device *dev, uint16_t bit_rate, uint8_t node_id) {
	CO_ReturnError_t err;
	uint32_t heapMemoryUsed;
	uint32_t errInfo = 0;

	if (!device_is_ready(dev)) {
		printf("CAN interface not ready");
		return -1;
	}

	/* Allocate memory */
	CO_config_t* config_ptr = NULL;
	CO = CO_new(config_ptr, &heapMemoryUsed);
	if (CO == NULL) {
		printf("Can't allocate memory\n");
		return -1;
	} else {
		printf("Allocated %u bytes for CANopen objects\n", heapMemoryUsed);
	}

	/* CANopen communication reset - initialize CANopen objects */
	printf("CANopenNode - Reset communication...\n");

	/* Wait rt_thread. */
	CO->CANmodule->CANnormal = false;

	/* Enter CAN configuration. */
	CO_CANsetConfigurationMode((void*)dev);
	CO_CANmodule_disable(CO->CANmodule);

	/* initialize CANopen */
	err = CO_CANinit(CO, (void*)dev, bit_rate);
	if (err != CO_ERROR_NO) {
		printf("CAN initialization failed: %d\n", err);
		return -1;
	}

	err = CO_CANopenInit(CO,                   /* CANopen object */
											 NULL,                 /* alternate NMT */
											 NULL,                 /* alternate em */
											 OD,                   /* Object dictionary */
											 OD_STATUS_BITS,       /* Optional OD_statusBits */
											 NMT_CONTROL,          /* CO_NMT_control_t */
											 FIRST_HB_TIME,        /* firstHBTime_ms */
											 SDO_SRV_TIMEOUT_TIME, /* SDOserverTimeoutTime_ms */
											 SDO_CLI_TIMEOUT_TIME, /* SDOclientTimeoutTime_ms */
											 SDO_CLI_BLOCK,        /* SDOclientBlockTransfer */
											 node_id, &errInfo);
	if (err != CO_ERROR_NO && err != CO_ERROR_NODE_ID_UNCONFIGURED_LSS) {
		if (err == CO_ERROR_OD_PARAMETERS) {
			printf("Object Dictionary entry 0x%X\n", errInfo);
		} else {
			printf("CANopen initialization failed: %d\n", err);
		}
		return -1;
	}

	err = CO_CANopenInitPDO(CO, CO->em, OD, node_id, &errInfo);
	if (err != CO_ERROR_NO) {
		if (err == CO_ERROR_OD_PARAMETERS) {
			printf("Object Dictionary entry 0x%X\n", errInfo);
		} else {
			printf("PDO initialization failed: %d\n", err);
		}
		return -1;
	}

	/* Configure Timer interrupt function for execution every 1 millisecond */

	/* Configure CAN transmit and receive interrupt */

	if (CO->nodeIdUnconfigured) {
		printf("CANopenNode - Node-id not initialized\n");
	}

	/* start CAN */
	CO_CANsetNormalMode(CO->CANmodule);

	/* start threads */
	co_main_tid = k_thread_create(&co_main_thread_data, co_main_stack_area,
																K_THREAD_STACK_SIZEOF(co_main_stack_area),
																co_main_thread,
																NULL, NULL, NULL,
																CO_MAIN_PRIORITY, 0, K_NO_WAIT);
	co_rt_tid = k_thread_create(&co_rt_thread_data, co_rt_stack_area,
															K_THREAD_STACK_SIZEOF(co_rt_stack_area),
															co_rt_thread,
															NULL, NULL, NULL,
															CO_RT_PRIORITY, 0, K_NO_WAIT);

	return 0;
}

bool canopennode_is_running(void) {
	return reset == CO_RESET_NOT;
}

void canopennode_stop(const struct device *dev) {
	/* stop threads */
	reset = CO_RESET_QUIT;

	/* delete objects from memory */
	if (dev != NULL) {
		CO_CANsetConfigurationMode((void*)dev);
	}
	CO_delete(CO);

	printf("CANopenNode finished\n");
}

static void co_main_thread(void *p1, void *p2, void *p3) {
	(void)p1;
	(void)p2;
	(void)p3;
	reset = CO_RESET_NOT;

	printf("CANopenNode - Running...\n");

	int64_t timestamp;
	uint32_t time_diff_us = 0U;
	uint32_t time_next_us = 1000U;
	while (reset == CO_RESET_NOT) {
		time_next_us = 1000U;
		timestamp = k_uptime_get();
		reset = CO_process(CO, false, time_diff_us, &time_next_us);
		if (time_next_us > 0) {
			k_sleep(K_MSEC(time_next_us / 1000));
			time_diff_us = (uint32_t)k_uptime_delta(&timestamp) * 1000;
		} else {
			// do not sleep, more processing to be done by the stack
			time_diff_us = 0;
		}
	}
}

/* timer thread executes in constant intervals */
static void co_rt_thread(void *p1, void *p2, void *p3) {
	(void)p1;
	(void)p2;
	(void)p3;

	while (reset == CO_RESET_NOT) {
		k_sleep(K_MSEC(1));

		CO_LOCK_OD(CO->CANmodule);
		if (!CO->nodeIdUnconfigured && CO->CANmodule->CANnormal) {
			bool syncWas = false;
			/* get time difference since last function call */
			uint32_t timeDifference_us = 1000;

#if (CO_CONFIG_SYNC) & CO_CONFIG_SYNC_ENABLE
			syncWas = CO_process_SYNC(CO, timeDifference_us, NULL);
#endif
#if (CO_CONFIG_PDO) & CO_CONFIG_RPDO_ENABLE
			CO_process_RPDO(CO, syncWas, timeDifference_us, NULL);
#endif
#if (CO_CONFIG_PDO) & CO_CONFIG_TPDO_ENABLE
			CO_process_TPDO(CO, syncWas, timeDifference_us, NULL);
#endif
		}
		CO_UNLOCK_OD(CO->CANmodule);
	}
}
