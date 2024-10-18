#ifndef ORESAT_H
#define ORESAT_H

#include <CANopen.h>
#include <OD.h>
#ifdef STM32F091xC
#include "stm32f091xc.h"
#endif

#define DEFAULT_NODE_ID 0x7C

static inline uint8_t oresat_get_node_id(void)
{
	uint8_t node_id = DEFAULT_NODE_ID;
#ifdef STM32F091xC
	/* node id is set in the stm32f091rc's option bytes. */
	node_id = (FLASH->OBR & FLASH_OBR_DATA0_Msk) >> FLASH_OBR_DATA0_Pos;
	if (node_id == 0 || node_id > DEFAULT_NODE_ID) {
		node_id = DEFAULT_NODE_ID;
	}
#endif
	return node_id;
}

/** add node id to sdo cob ids */
static inline void oresat_fix_cob_ids_in_od(uint8_t node_id)
{
	uint32_t cob_id;
	OD_entry_t *entry;
#if OD_CNT_RPDO > 0
	for (int i = 0; i < OD_CNT_RPDO; i++) {
		entry = OD_find(OD, 0x1400 + i);
		if (!entry) {
			continue;
		}
		OD_get_u32(entry, 1, &cob_id, true);
		if (cob_id == 0x200U + (0x100U * (i % 4))) {
			OD_set_u32(entry, 1, cob_id + node_id + (i / 4), true);
		}
	}
#endif
#if OD_CNT_TPDO > 0
	for (int i = 0; i < OD_CNT_TPDO; i++) {
		entry = OD_find(OD, 0x1800 + i);
		if (!entry) {
			continue;
		}
		OD_get_u32(entry, 1, &cob_id, true);
		if (cob_id == 0x180U + (0x100U * (i % 4))) {
			OD_set_u32(entry, 1, cob_id + node_id + (i / 4), true);
		}
	}
#endif
}

#endif
