#ifndef ORESAT_H
#define ORESAT_H

#include "stm32f091xc.h"

static inline uint8_t get_node_id(void) {
	uint8_t node_id = READ_BIT(FLASH->OBR, FLASH_OBR_DATA0);
	if (node_id == 0 || node_id > 0x7C)  {
		node_id = 0x7C;
	}
  return node_id;
}

#endif
