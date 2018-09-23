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

#ifdef CFG_TSEE_LINUX
#include <linux/mm.h>
#include <asm/pgtable.h>
#include <asm/smp.h>
#include <asm/sections.h>
#include <linux/tsee/tsee_entry.h>

#else


#endif

#ifdef CONFIG_TRUSTKERNEL_TSEE_RTKS

/* Extra memory needed by TSEE */
void* tsee_rtks_extra_mem = 0;

int tsee_rtks_support_large_memory;

u8 tsee_rtks_started = 0;

EXPORT_SYMBOL(tsee_rtks_extra_mem);
EXPORT_SYMBOL(tsee_rtks_support_large_memory);
EXPORT_SYMBOL(tsee_rtks_started);

__attribute__((section(".tsee_rtks.bitmap"))) u8 tsee_rtks_pgt_bitmap_arr[TSEE_RTKS_PGT_BITMAP_LEN] = {0}; 
__attribute__((section(".tsee_rtks.dblmap"))) u8 tsee_rtks_map_bitmap_arr[TSEE_RTKS_PGT_BITMAP_LEN] = {0}; 

extern u8 tsee_rtks_pgt_bitmap[];
extern u8 tsee_rtks_map_bitmap[];

#if defined(CONFIG_TRUSTKERNEL_TSEE_RTKS_TZ) || defined(CONFIG_TRUSTKERNEL_TSEE_RTKS_KEE) || defined(CONFIG_TRUSTKERNEL_TSEE_RTKS_VMM)

void tsee_rtks_init(void) 
{
	tsee_rtks_init_t init;
	init.magic = TSEE_RTKS_INIT_MAGIC;
	init.vmalloc_start = VMALLOC_START;
	init.vmalloc_end = (u64)high_memory;
	init.init_mm_pgd = (u64)__pa(swapper_pg_dir);
	init.id_map_pgd = (u64)__pa(idmap_pg_dir);
	init.tsee_rtks_pgt_bitmap = (u64)__pa(tsee_rtks_pgt_bitmap);
	init.tsee_rtks_map_bitmap = (u64)__pa(tsee_rtks_map_bitmap);
	init.tsee_rtks_pgt_bitmap_size = TSEE_RTKS_PGT_BITMAP_LEN;
	init.zero_pg_addr = page_to_phys(empty_zero_page);
	init._text = (u64) _text;//
	init._etext = (u64) _etext;
	if (!tsee_rtks_extra_mem) {
		printk(KERN_ERR"Disable tsee_rtks: Failed to allocate extra mem\n");
		return;
	}
	init.extra_memory_addr = __pa(tsee_rtks_extra_mem);
	init.extra_memory_size = TSEE_RTKS_VMM_BUFFER_SIZE;
	init._srodata = (u64) __start_rodata;
	init._erodata =(u64) __end_rodata;
	init.large_memory = tsee_rtks_support_large_memory;

	printk("initing tsee real-time kernel protection...\n");

	tsee_rtks_call(TSEE_RTKS_INIT, (u64)&init, 0, 0, 0, 0);

	tsee_rtks_started = 1;
}
EXPORT_SYMBOL(tsee_rtks_init);

#endif  


#endif /* CONFIG_TRUSTKERNEL_TSEE_RTKS */
