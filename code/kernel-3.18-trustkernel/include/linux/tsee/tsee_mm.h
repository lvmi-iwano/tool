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

#ifdef CONFIG_TRUSTKERNEL_TSEE_RTKS

#ifndef __ASSEMBLY__

#include <asm/pgtable.h>
//#include <asm/tlb.h>
#include <linux/tsee/tsee_entry.h>

#define pmd_block(pmd)      ((pmd_val(pmd) & 0x3)  == 1)
#define PTE_VALID		(_AT(pteval_t, 1) << 0)
#define pte_valid_not_user(pte) \
	((pte_val(pte) & (PTE_VALID | PTE_USER)) == PTE_VALID)


#define tsee_rtks_is_pg_protected(va) tsee_rtks_is_protected(va,__pa(va),(u64 *)tsee_rtks_pgt_bitmap,1)
#define tsee_rtks_is_pg_dbl_mapped(pa) tsee_rtks_is_protected((u64)__va(pa),pa,(u64 *)tsee_rtks_map_bitmap,0)

#define TSEE_RTKS_PHYS_OFFSET_MAX     (0x900000ULL << PAGE_SHIFT)
#define TSEE_RTKS_PHYS_ADDR_MASK      ((1ULL << 40)-1)

extern u8 tsee_rtks_pgt_bitmap[];
extern u8 tsee_rtks_map_bitmap[];

extern void panic(const char *fmt, ...);


static inline u8 tsee_rtks_is_protected(u64 va,u64 pa, u64 *base_addr,u64 type)
{
	u64 phys_addr = pa & (TSEE_RTKS_PHYS_ADDR_MASK);
	u64 index = (phys_addr - PHYS_OFFSET) >> PAGE_SHIFT;
	u64 *p = base_addr;
	u64 tmp = (index>>6);//?
	u64 rindex;
	u8 val;


	/* We are going to ignore if input address NOT belong to DRAM area */
	if((phys_addr < PHYS_OFFSET) || (index ==(~0ULL)) ||
	   (phys_addr  > TSEE_RTKS_PHYS_OFFSET_MAX)) {
		return 0;
	}

	p += (tmp);
	rindex = index % 64;
	val = (((*p) & (1ULL<<rindex))?1:0);
	return val;
}

static inline void tsee_rtks_set_pud(pud_t *pudp, pud_t pud)
{
	if (tsee_rtks_is_pg_protected((u64)pudp)) {
		tsee_rtks_call(TSEE_RTKS_PGD_SET, (unsigned long)pudp, pud_val(pud), 0, 0, 0);
	} else {
		asm volatile("mov x1, %0\n"
					 "mov x2, %1\n"
					 "str x2, [x1]\n"
					 :
					 : "r" (pudp), "r" (pud)
					 : "x1", "x2", "memory" );
		dsb(ishst);
		isb();
	}
}

static inline void tsee_rtks_set_pmd(pmd_t *pmdp, pmd_t pmd)
{
	if (tsee_rtks_is_pg_protected((u64)pmdp)) {
		tsee_rtks_call(TSEE_RTKS_PMD_SET, (unsigned long)pmdp, pmd_val(pmd), 0, 0, 0);
	} else {
		asm volatile("mov x1, %0\n"
					 "mov x2, %1\n"
					 "str x2, [x1]\n"
					 :
					 : "r" (pmdp), "r" (pmd)
					 : "x1", "x2", "memory" );
		dsb(ishst);
		isb();
	}
}

static inline void tsee_rtks_set_pte(pte_t *ptep, pte_t pte)
{
	if (pte && tsee_rtks_is_pg_dbl_mapped((u64)(pte)) ) {
		panic("TrustKernel RTKS : Double mapping Detected pte = 0x%llx ptep = %p",(u64)pte, ptep);
		return;
	}
	if (tsee_rtks_is_pg_protected((u64)ptep)) {
		tsee_rtks_call(TSEE_RTKS_PTE_SET, (unsigned long)ptep, pte_val(pte), 0, 0, 0);
	} else {
		asm volatile("mov x1, %0\n"
					 "mov x2, %1\n"
					 "str x2, [x1]\n"
					 :
					 : "r" (ptep), "r" (pte)
					 : "x1", "x2", "memory" );
		if (pte_valid_not_user(pte)) {
			dsb(ishst);
			isb();
		}
	}
}

#if 0
static inline void __tsee_rtks_pmd_free_tlb(struct mmu_gather *tlb, pmd_t *pmdp,
								  unsigned long addr)
{
	if ((unsigned long)pmdp >= (unsigned long)TSEE_RTKS_ROBUF_VA && (unsigned long)pmdp < ((unsigned long)TSEE_RTKS_ROBUF_VA + TSEE_RTKS_ROBUF_SIZE))
		tsee_rtks_ro_free((void*)pmdp);
	else {
		__flush_tlb_pgtable(tlb->mm, addr);
		tlb_remove_entry(tlb, virt_to_page(pmdp));
	}
}

static inline void __tsee_rtks_pud_free_tlb(struct mmu_gather *tlb, pud_t *pudp,
								  unsigned long addr)
{
	if ((unsigned long)pudp >= (unsigned long)TSEE_RTKS_ROBUF_VA && (unsigned long)pudp < ((unsigned long)TSEE_RTKS_ROBUF_VA + TSEE_RTKS_ROBUF_SIZE))
		tsee_rtks_ro_free((void*)pudp);
	else {
		__flush_tlb_pgtable(tlb->mm, addr);
		tlb_remove_entry(tlb, virt_to_page(pudp));
	}
}
#endif

#endif 

#endif /* CONFIG_TRUSTKERNEL_TSEE_RTKS */

#endif /* __TSEE_MM_H_ */

