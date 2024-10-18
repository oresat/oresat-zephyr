#ifndef ORESAT_H
#define ORESAT_H

#ifdef STM32F091xC
#include "stm32f091xc.h"
#endif

static inline uint8_t get_node_id(void)
{
#ifdef STM32F091xC
	uint8_t node_id = (FLASH->OBR & FLASH_OBR_DATA0_Msk) >> FLASH_OBR_DATA0_Pos;
	if (node_id == 0 || node_id > 0x7C) {
		node_id = 0x7C;
	}
	return node_id;
#else
	return 0x7c;
#endif
}

#endif
