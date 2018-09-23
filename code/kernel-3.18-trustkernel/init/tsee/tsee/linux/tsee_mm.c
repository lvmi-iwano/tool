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
#include <linux/export.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <asm/pgtable.h>
#include <asm/pgalloc.h>
#include <linux/tsee/tsee_entry.h>

#ifdef CONFIG_TRUSTKERNEL_TSEE_RTKS

extern void* tsee_rtks_extra_mem;
#ifdef CFG_TSEE_LINUX
static spinlock_t ro_tsee_rtks_pages_lock = __SPIN_LOCK_UNLOCKED();
#endif
static char ro_pages_stat[TSEE_RTKS_RO_PAGES] = {0};
static unsigned ro_alloc_last = 0;

void *tsee_rtks_ro_alloc(void)
{
	unsigned long flags;
	int i, j;
	void * alloc_addr = NULL;

#ifdef CFG_TSEE_LINUX
	spin_lock_irqsave(&ro_tsee_rtks_pages_lock,flags);
#endif

	for (i = 0, j = ro_alloc_last; i < (TSEE_RTKS_RO_PAGES) ; i++) {
		j =  (j+i) %(TSEE_RTKS_RO_PAGES);
		if (!ro_pages_stat[j]) {
			ro_pages_stat[j] = 1;
			ro_alloc_last = j+1;
			alloc_addr = (void*) ((u64)TSEE_RTKS_ROBUF_VA +  (j << PAGE_SHIFT));
			break;
		}
	}
#ifdef CFG_TSEE_LINUX
	spin_unlock_irqrestore(&ro_tsee_rtks_pages_lock,flags);
#endif

	return alloc_addr;
}

void tsee_rtks_ro_free(void *free_addr)
{
	int i;
	unsigned long flags;

	i =  ((u64)free_addr - (u64)TSEE_RTKS_ROBUF_VA) >> PAGE_SHIFT;
#ifdef CFG_TSEE_LINUX
	spin_lock_irqsave(&ro_tsee_rtks_pages_lock,flags);
#endif
	ro_pages_stat[i] = 0;
	ro_alloc_last = i;
#ifdef CFG_TSEE_LINUX
	spin_unlock_irqrestore(&ro_tsee_rtks_pages_lock,flags);
#endif
}

unsigned int is_tsee_rtks_ro_page(u64 addr)
{
	if( (addr >= (u64)TSEE_RTKS_ROBUF_VA)
		&& (addr < (u64)(TSEE_RTKS_ROBUF_VA+ TSEE_RTKS_ROBUF_SIZE)))
		return 1;
	else return 0;
}

#else 
#include <stdint.h>
#include <tee/tsee/defs.h>
#include <tee/tsee/tsee_entry.h>

#endif



#endif /* CONFIG_TRUSTKERNEL_TSEE_RTKS */
