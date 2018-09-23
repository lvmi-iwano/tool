/*
 *
 * Copyright (C) 2015 - 2017 TrustKernel Team, Shanghai Pingbo Information Technology Corporation Limited - All Rights Reserved
 *
 *      https://www.trustkernel.com
 *
 * This file is part of T6 and TSEE.
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 */

#ifndef __TSEE_MM_H_
#define __TSEE_MM_H_

// TEE virtual address base, differs across TEEs
#define KERNBASE        0x80000000         // First kernel virtual address
#define BOARD_BASE      0x50100000		   // TEE physical entry address

#ifndef __ASSEMBLY__

#include <tee/tsee/tsee_entry.h>
#include <tee/tsee/defs.h>

extern tsee_rtks_init_t tsee_init;

static inline uint32_t tee_virt_to_phys(void *a) {
	return ((uint32_t) (a))  - KERNBASE + BOARD_BASE;
}
static inline uint32_t tee_phys_to_virt(uint32_t a) {
	return ((a) + KERNBASE - BOARD_BASE);
}

#endif 

#endif /* __TSEE_MM_H_ */

