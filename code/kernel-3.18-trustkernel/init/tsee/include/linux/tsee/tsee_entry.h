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

#ifndef __TSEE_ENTRY_H_
#define __TSEE_ENTRY_H_

#ifndef __ASSEMBLY__

#define TSEE_RTKS_INIT_MAGIC 		0x52544b53
#define TSEE_RTKS_VMM_BUFFER_SIZE	0x400000

#define TSEE_RTKS_PREFIX  			(0xdeadbe00UL)
#define TSEE_RTKS_CMDID(__CMD_ID) 	(unsigned long)(((__CMD_ID) & 0xff) | TSEE_RTKS_PREFIX)

#define TSEE_RTKS_INIT     			TSEE_RTKS_CMDID(0)
#define TSEE_RTKS_DEF_INIT     		TSEE_RTKS_CMDID(1)

#define TSEE_RTKS_PGD_SET  			TSEE_RTKS_CMDID(0x21)
#define TSEE_RTKS_PMD_SET  			TSEE_RTKS_CMDID(0x22)
#define TSEE_RTKS_PTE_SET  			TSEE_RTKS_CMDID(0x23)
#define TSEE_RTKS_PGD_FREE 			TSEE_RTKS_CMDID(0x24)
#define TSEE_RTKS_PGD_NEW  			TSEE_RTKS_CMDID(0x25)

#define TSEE_RTKS_PGT_BITMAP_LEN 	0x20000

// memory layout
#define   TVEE_START        	0xB0500000
#define   TVEE_SIZE         	1<<20

#define   TSEE_RTKS_ROBUF_START      0x44700000
#define   TSEE_RTKS_ROBUF_SIZE       0x800000
#define   TSEE_RTKS_RO_PAGES  		 0x800 // (TSEE_RTKS_ROBUF_SIZE/PAGE_SIZE)

#define TSEE_RTKS_ROBUF_VA      (phys_to_virt(TSEE_RTKS_ROBUF_START))

#define TSEE_RTKS_EXTRA_MEM_SIZE	0x600000

struct tsee_rtks_init {
	u32 magic;
	u64 vmalloc_start;
	u64 vmalloc_end;
	u64 init_mm_pgd;
	u64 id_map_pgd;
	u64 zero_pg_addr;
	u64 tsee_rtks_pgt_bitmap;
	u64 tsee_rtks_map_bitmap;
	u32 tsee_rtks_pgt_bitmap_size;
	u64 _text;
	u64 _etext;
	u64 extra_memory_addr;
	u32 extra_memory_size;
	u64 physmap_addr;
	u64 _srodata;
	u64 _erodata;
	u32 large_memory;
} __attribute__((packed));

typedef struct tsee_rtks_init tsee_rtks_init_t;


void tsee_rtks_init(void);

void tsee_rtks_call(unsigned long long cmd, unsigned long long arg0, unsigned long long arg1, 
					unsigned long long arg2, unsigned long long arg3, unsigned long long arg4);

void *tsee_rtks_ro_alloc(void);

void tsee_rtks_ro_free(void *free_addr);

unsigned int is_tsee_rtks_ro_page(u64 addr);

#endif /* __ASSEMBLY__ */
#endif /* __TSEE_ENTRY_H_ */
