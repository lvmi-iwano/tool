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

#include <console.h>
#include <trace.h>
#include <stdint.h>
#include <string.h>
#include <tee/tsee/tsee_entry.h>
#include <tee/tsee/defs.h>
#include <tsee_sm.h>
#include <platform_config.h>

typedef void (*tee_entry_call) (u32, u32, u32);
extern void* _binary_tee_img_start;
extern void* _binary_tee_img_end;
extern tsee_rtks_init_t tsee_init;

extern uint32_t _tsee_pgtbl;
extern uint32_t tsee_rtks_map_bitmap;
extern uint32_t tsee_rtks_map_bitmap_end;
uint32_t *tsee_pgtbl = &_tsee_pgtbl;
extern void flush_tlb_all();
extern void flush_cache_all();

// section type
void set_bootpgtbl (uint32_t virt, uint32_t phy, uint32_t len, int dev_mem )
{
    uint32_t pde;
    int		idx;

    // convert all the parameters to indexes
    virt >>= PDE_SHIFT;
    phy  >>= PDE_SHIFT;
    len  >>= PDE_SHIFT;

    for (idx = 0; idx < len; idx++) {
        pde = (phy << PDE_SHIFT);

        if (!dev_mem) {
            // normal memory, make it kernel-only, cachable, bufferable
            pde |= (0x1 << 10) | (1 << 3) | (1 <<2) | 0x2;
        } else {
            // device memory, make it non-cachable and non-bufferable
            pde |= (0x1 << 10) | 0x2;
        }

        tsee_pgtbl[virt] = pde;

        virt++;
        phy++;
    }
}
void load_pgtlb (uint32_t * kernel_pgtbl)
{
    uint32_t ret;
    char 	 arch;
    uint32_t val;
    
	// we need to check the cache/tlb etc., but let's skip it for now 
	val = 0;
	asm("DMB");
	asm("mcr p15, 0, %[v], c7, c5, 0": :[v]"r" (val):);//clear icache
	//flush tlb
	flush_tlb_all();
	flush_cache_all();
	asm("DMB");
	asm("ISB");

    // set domain access control: all domain will be checked for permission in the TLB entry
    val = 0x55555555;
    asm("MCR p15, 0, %[v], c3, c0, 0": :[v]"r" (val):); //write domain access control register

    // set the page table base registers. We use two page tables: TTBR0
    // for user space and TTBR1 for kernel space
	//write to translation table control register: 
	//ttbr0 page table boundary size is set to 1KB(32-28), which means if virtual address of [31:28] are all 0, use ttbr0, otherwise ttbr1
    val = 0;
    asm("MCR p15, 0, %[v], c2, c0, 2": :[v]"r" (val):);

    // set the kernel page table: ttbr0
    val = (uint32_t)kernel_pgtbl | 0x00;
    asm("MCR p15, 0, %[v], c2, c0, 0": :[v]"r" (val):);

    // ok, enable paging using read/modify/write 
    asm("MRC p15, 0, %[r], c1, c0, 0": [r]"=r" (val)::);

	val |= 0x00000001; //enable MMU
	//val |= (1<<2); //enable dcache
	//val |= (1<<12);//enable icache 
	//val |= (1<<11);//enable Branch prediction 

	//setting the control register
    asm("MCR p15, 0, %[r], c1, c0, 0": :[r]"r" (val):);
}

void start_secondary(void) {
	tee_entry_call tee_fn;
	DMSG("Loading TSEE page table\n");
	load_pgtlb (tsee_pgtbl);
	tee_fn = ((u32 *)&_binary_tee_img_start);
	DMSG("Secondary Jumping to TEE");
	// jump to TEE
	tee_fn(0, 0, 0);
}

extern uint32_t trap_entry;

void start (void) {
	tee_entry_call tee_fn;
	extern pt_state_t tsee_pt;

	memset(&tsee_init, 0, sizeof(tsee_init));

	tsee_init.tsee_rtks_phymap = &tsee_rtks_map_bitmap;
	memset((char*)tsee_init.tsee_rtks_phymap, 0, &tsee_rtks_map_bitmap_end - &tsee_rtks_map_bitmap);

	// main memory
    set_bootpgtbl(DRAM0_BASE, DRAM0_BASE, DRAM0_SIZE, 0);
    set_bootpgtbl(DRAM1_BASE, DRAM1_BASE, DRAM1_SIZE, 0);
	// device memory
    set_bootpgtbl(DEVRAM_BASE, DEVRAM_BASE, DEVRAM_SIZE, 1);

	DMSG("Loading TSEE page table\n");
	load_pgtlb (tsee_pgtbl);
	// save TSEE pt states 
	asm("MRC p15, 0, %[r], c2, c0, 2": [r]"=r" (tsee_pt.ttbcr)::);
    asm("MRC p15, 0, %[r], c2, c0, 0": [r]"=r" (tsee_pt.ttbr0)::);
    asm("MRC p15, 0, %[r], c2, c0, 1": [r]"=r" (tsee_pt.ttbr1)::);
    asm("MRC p15, 0, %[r], c1, c0, 0": [r]"=r" (tsee_pt.sctlr)::);
	tsee_pt.vbar = &trap_entry;
	DMSG("TSEE: 0x%x-0x%x-0x%x-0x%x-0x%x-0x%x\n", tsee_pt.ttbcr, tsee_pt.ttbr1, tsee_pt.ttbr0, tsee_pt.sctlr, tsee_pt.sp, tsee_pt.vbar);


	tee_fn = ((u32 *)&_binary_tee_img_start);
	DMSG("Primary Jumping to TEE");
	DMSG("TEE load addr: 0x%x\n", (u32)tee_fn);
	// jump to TEE
	tee_fn(0, 0, 0);

	// stack will be reset afterwards
}


