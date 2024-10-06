#ifndef CO_DRIVER_CUSTOM_H
#define CO_DRIVER_CUSTOM_H

/* NMT configs */
#undef CO_CONFIG_NMT_MASTER
#define CO_CONFIG_NMT_MASTER 0

/* HB configs */
#undef CO_CONFIG_HB_CONS
#define CO_CONFIG_HB_CONS 0

/* EM configs */
#undef CO_CONFIG_EM_CONSUMER
#define CO_CONFIG_EM_CONSUMER 0

/* TIME configs */
#undef CO_CONFIG_TIME
#define CO_CONFIG_TIME 0
#undef CO_CONFIG_TIME_ENABLE
#define CO_CONFIG_TIME_ENABLE 0

/* OD storage configs */
#undef CO_CONFIG_STORAGE_ENABLE
#define CO_CONFIG_STORAGE_ENABLE 0

/* SDO configs */
#undef CO_CONFIG_SDO_CLI
#define CO_CONFIG_SDO_CLI 0

/* PDO configs */
#undef CO_CONFIG_PDO
#define CO_CONFIG_PDO CO_CONFIG_TPDO_ENABLE

/* LEDS configs */
#undef CO_CONFIG_LEDS_ENABLE
#define CO_CONFIG_LEDS_ENABLE 0

/* GTC configs */
#undef CO_CONFIG_GTC_ENABLE
#define CO_CONFIG_GTC_ENABLE 0

/* SRDO configs */
#undef CO_CONFIG_SRDO_ENABLE
#define CO_CONFIG_SRDO_ENABLE 0

/* LSS configs */
#undef CO_CONFIG_LSS_SLAVE
#define CO_CONFIG_LSS_SLAVE 0
#undef CO_CONFIG_LSS_MASTER
#define CO_CONFIG_LSS_MASTER 0

/* GTW configs */
#undef CO_CONFIG_GTW
#define CO_CONFIG_GTW 0

/* Trace configs */
#undef CO_CONFIG_TRACE_ENABLE
#define CO_CONFIG_TRACE_ENABLE 0

/* LSS config */
#undef CO_CONFIG_LSS
#define CO_CONFIG_LSS 0

#endif /* CO_DRIVER_CUSTOM_H */
