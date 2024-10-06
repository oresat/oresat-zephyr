#ifndef _CANOPENNODE_H_
#define _CANOPENNODE_H_

#include <CANopen.h>

extern CO_t *CO;

int canopennode_init(const struct device *dev, uint16_t bit_rate, uint8_t node_id);
bool canopennode_is_running(void);
void canopennode_stop(const struct device *dev);

#endif
