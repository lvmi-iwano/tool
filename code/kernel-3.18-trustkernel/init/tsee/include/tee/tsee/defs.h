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

#ifndef TSEE_TEE_DEFS_H
#define TSEE_TEE_DEFS_H

#define PAGE_SHIFT	12
#define PAGE_SIZE	(1 << PAGE_SHIFT)
#define PAGE_MASK	(~((1 << PAGE_SHIFT) - 1))
#define SECTION_SHIFT	20
#define SECTION_SIZE (1 << SECTION_SHIFT)

#define PE_TYPES    		0x03    // mask for page type
#define PDE_SECTION_TYPE   	0x02    // use "section" type
#define PDE_PAGE_TYPE   	0x01    // use "page" type
#define PTE_LARGE_TYPE   	0x01    // use "lage page" type
#define PTE_SMALL_TYPE		0x2		// use "small page" type

#define PDE_SHIFT   20
#define PTE_SHIFT   12                  // shift how many bits to get PTE index
#define NUM_PTE     (1 << (PDE_SHIFT - PTE_SHIFT))  // how many PTE in a PT
#define PDE_IDX(v)  ((uint32_t)(v) >> PDE_SHIFT) // index for page table entry
#define PTE_IDX(v)  (((uint32_t)(v) >> PTE_SHIFT) & (NUM_PTE - 1))
#define PT_SZ       (NUM_PTE << 2)                  // user page table size (1K)
#define PT_ADDR(v)  align_dn(v, PT_SZ)              // physical address of the PT

#define align_up(sz, al) (((uint32_t)(sz)+ (uint32_t)(al)-1) & ~((uint32_t)(al)-1))
#define align_dn(sz, al) ((uint32_t)(sz) & ~((uint32_t)(al)-1))

#endif
