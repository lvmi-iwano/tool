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

#ifndef _TSEE_SM_H_
#define _TSEE_SM_H_

// DO NOT change the order
struct pt_state{
	uint32_t ttbcr;
	uint32_t ttbr0;
	uint32_t ttbr1;
	uint32_t sctlr;
	uint32_t sp;
	uint32_t vbar;
};

typedef struct pt_state pt_state_t;


void sm_write_mvbar(void* handler);
void sm_lock_down(void);
int is_sm_lock_down(void);
void sm_tee_deinit(void);
int is_tsee_call(uint32_t prefix) __attribute__((section(".text.tsee_ro")));
void map_tsee(void);
void unmap_tsee(void);
void mem_enforce_perm(uint32_t start, uint32_t end, uint32_t apx, uint32_t ap, uint32_t xn, uint32_t pxn);
void tlb_lockdown(uint32_t addr);

#endif
