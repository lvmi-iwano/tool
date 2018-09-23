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

#include <stdint.h>
#include <string.h>
#include <trace.h>
#include <tee/tsee/barrier.h>
#include <tee/tsee/tsee_entry.h>
#include <tsee_sm.h>
#include <tee/tsee/tsee_mm.h>
#include <platform_config.h>

extern void flush_tlb_all();
extern pt_state_t tee_pt;

tsee_rtks_init_t tsee_init;

static int is_tsee_range(uint32_t pa) {
	if(pa >= CFG_TSEE_LOAD_ADDR && pa <= (uint32_t)(CFG_TSEE_LOAD_ADDR + CFG_TSEE_RAM_SIZE)) {
		return 1;
	}
	return 0;
}

static int is_tee_range(uint32_t pa) {
	if((pa >= tsee_init.kmem_start && pa < tsee_init.kmem_end) ||
	   (pa >= tsee_init.kmem_start2 && pa < tsee_init.kmem_end2)) {
		return 1;
	}
	return 0;
}
static int is_l2pgtbl_range(uint32_t pa) {
	if(pa >= tsee_init.l2pgtbl_start && pa < tsee_init.l2pgtbl_end) {
		return 1;
	}
	return 0;
}
static int is_kpt_range(uint32_t pa) {
	if((pa >= tsee_init.kpt_start && pa < tsee_init.kpt_end) ||
	   (pa >= tsee_init.kpt_start2 && pa < tsee_init.kpt_end2)) {
		return 1;
	}
	return 0;
}
static int is_reeshr_range(uint32_t pa) {
	if(pa >= tsee_init.reemem_shr_start && pa < tsee_init.reemem_shr_end) {
		return 1;
	}
	return 0;
}
static int is_kpgtbl_range(uint32_t pa) {
	if(pa >= tsee_init.kpgtbl && pa < (tsee_init.kpgtbl + tsee_init.kpgtbl_size)) {
		return 1;
	}
	return 0;
}
static int is_kdata_range(uint32_t pa) {
	if((pa >= tsee_init.kdata_start && pa < tsee_init.kdata_end) ||
	   (pa >= tsee_init.kdata_start2 && pa < tsee_init.kdata_end2)) {
		return 1;
	}
	return 0;
}
static int is_krodata_range(uint32_t pa) {
	if(pa >= tsee_init.krodata_start && pa < tsee_init.krodata_end) {
		return 1;
	}
	return 0;
}
static int is_ktext_range(uint32_t pa) {
	if((pa >= tsee_init.ktext_start && pa < tsee_init.ktext_end) ||
	   (pa >= tsee_init.ktext_start2 && pa < tsee_init.ktext_end2)) {
		return 1;
	}
	return 0;
}
static int is_devmem_range(uint32_t pa) {
#ifndef DEVRAM_BASE
	return 0;
#else
	if(pa >= (uint32_t)DEVRAM_BASE && pa < (uint32_t)DEVRAM_SIZE)
		return 1;
	return 0;
#endif
}
static int is_dram0_ranges(uint32_t ps, uint32_t pe) {
	if(pe <= (uint32_t)DRAM0_BASE || (uint32_t)(DRAM0_BASE + DRAM0_SIZE) <= ps)
		return 0;
	return 1;
}
static int is_dram1_ranges(uint32_t ps, uint32_t pe) {
	if(pe <= (uint32_t)DRAM1_BASE || (uint32_t)(DRAM1_BASE + DRAM1_SIZE) <= ps)
		return 0;
	return 1;
}
static int is_devmem_ranges(uint32_t ps, uint32_t pe) {
	if(pe <= (uint32_t)DEVRAM_BASE || (uint32_t)(DEVRAM_BASE + DEVRAM_SIZE) <= ps)
		return 0;
	return 1;
}

__attribute__((section(".tsee_rtks.phymap"))) char tsee_rtks_phymap[TSEE_RTKS_PGT_BITMAP_LEN] = {0};

static uint32_t get_phymap_index(uint32_t pa) {
	uint32_t dram0 = 0, dram1 = 0, devram = 0;
	uint32_t index = 0;
	dram0 = DRAM0_SIZE >> PAGE_SHIFT;
	dram1 = DRAM1_SIZE >> PAGE_SHIFT;
	devram = DEVRAM_SIZE >> PAGE_SHIFT;

	if(is_dram0_ranges(pa, pa + PAGE_SIZE)) {
		index = (pa - DRAM0_BASE) >> PAGE_SHIFT;
	}else if(is_dram1_ranges(pa, pa + PAGE_SIZE)) {
		index = (pa - DRAM1_BASE) >> PAGE_SHIFT;
		index += (DRAM0_SIZE >> PAGE_SHIFT);
	}else if(is_devmem_ranges(pa, pa + PAGE_SIZE)) {
		index = (pa - DEVRAM_BASE) >> PAGE_SHIFT;
		index += (DRAM0_SIZE >> PAGE_SHIFT);
		index += (DRAM1_SIZE >> PAGE_SHIFT);
	}else {
		//EMSG("Invalid pa for phymap index: 0x%x\n", pa);
		return 0xFFFFFFFF;
	}
	return index;
}

static int tsee_mempg_protect(uint32_t pa) {
	uint32_t index;
	uint32_t tmp;
	uint32_t rindex;
	uint32_t *p;
	
	pa = pa & (~(1 << PAGE_SHIFT));
	index = get_phymap_index(pa);
	if(index == 0xFFFFFFFF)
		return 1;
	tmp = index >> 5;
	p = tsee_init.tsee_rtks_phymap;
	p += tmp;
	rindex = index % 32;

	*p = (*p) | (1 << rindex);

	return 0;
}
static int tsee_mempg_unprotect(uint32_t pa) {
	uint32_t index;
	uint32_t tmp;
	uint32_t rindex;
	uint32_t *p;
	
	pa = pa & (~(1 << PAGE_SHIFT));
	index = get_phymap_index(pa);
	if(index == 0xFFFFFFFF)
		return 1;
	tmp = index >> 5;
	p = tsee_init.tsee_rtks_phymap;
	p += tmp;
	rindex = index % 32;

	*p = (*p) & (~(1 << rindex));

	return 0;
}
static int tsee_is_mempg_protected(uint32_t pa) {
	uint32_t index;
	uint32_t tmp;
	uint32_t rindex;
	uint32_t *p;
	int val;
	
	pa = pa & (~(1 << PAGE_SHIFT));
	index = get_phymap_index(pa);
	if(index == 0xFFFFFFFF)
		return 0;
	tmp = index >> 5;
	p = tsee_init.tsee_rtks_phymap;
	p += tmp;
	rindex = index % 32;

	val = ((*p) & (1 << rindex)) ? 1:0;
	return val;
}

extern pt_state_t tsee_pt;
extern pt_state_t tee_pt;

int tsee_ns_main(uint32_t cmd, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
	uint32_t tmp;
	DMSG("tsee-ns-(0x%x)-0x%x-0x%x-0x%x-0x%x-0x%x)\n", cmd, arg0, arg1, arg2, arg3, arg4);
	switch(cmd) {
		case TSEE_RTKS_MEM_PROTECT_NS:
			// Once a page is protected, any further mapping from TEE will be denied
			DMSG("tsee rtks mem map protection ns: 0x%x-0x%x\n", arg0, arg0 + arg1);
			// check that the range is not in TEE/REEShr ranges
			if(((arg0 >= tsee_init.kmem_end) || (arg0 + arg1 <= tsee_init.kmem_start)) &&
			   ((arg0 >= tsee_init.kmem_end2) || (arg0 + arg1 <= tsee_init.kmem_start2)) &&
			   ((arg0 >= tsee_init.reemem_shr_end) || (arg0 + arg1 <= tsee_init.reemem_shr_start)) ) {
				for(tmp = arg0; tmp < arg0 + arg1; tmp += PAGE_SIZE) {
					if(tsee_mempg_protect(tmp) != 0) {
						EMSG("protect 0x%x failed\n", tmp);
					}
				}
				//TODO: We need to remove existing mapping of the protected memory from TEE/TA
			}else {
				EMSG("illegal parameters for NS mem protection: 0x%x-0x%x \n", arg0, arg0 + arg1);
			}
			break;
		case TSEE_RTKS_MEM_UNPROTECT_NS:
			DMSG("tsee rtks mem map unprotection ns: 0x%x-0x%x\n", arg0, arg0 + arg1);
			// check that the range is not in TEE/REEShr ranges
			if(((arg0 >= tsee_init.kmem_end) || (arg0 + arg1 <= tsee_init.kmem_start)) &&
			   ((arg0 >= tsee_init.kmem_end2) || (arg0 + arg1 <= tsee_init.kmem_start2)) &&
			   ((arg0 >= tsee_init.reemem_shr_end) || (arg0 + arg1 <= tsee_init.reemem_shr_start)) ) {
				for(tmp = arg0; tmp < arg0 + arg1; tmp += PAGE_SIZE) {
					if(tsee_mempg_unprotect(tmp) != 0) {
						EMSG("unprotect 0x%x failed\n", tmp);
					}
				}
			}else {
				EMSG("illegal parameters for NS mem unprotection: 0x%x-0x%x \n", arg0, arg0 + arg1);
			}
			break;
		default:
			DMSG("TSEE-NS Error: unknown cmd for tsee: 0x%x-0x%x-0x%x-0x%x-0x%x-0x%x\n", cmd, arg0, arg1, arg2, arg3, arg4);
			break;
	}
	return 0;
}

int tsee_main(uint32_t cmd, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
	//DMSG("tsee(0x%x)-0x%x-0x%x-0x%x-0x%x-0x%x)\n", cmd, arg0, arg1, arg2, arg3, arg4);
	uint32_t tmp;
	//DMSG("TSEE: 0x%x-0x%x-0x%x-0x%x-0x%x-0x%x\n", tsee_pt.ttbcr, tsee_pt.ttbr1, tsee_pt.ttbr0, tsee_pt.sctlr, tsee_pt.sp, tsee_pt.vbar);
	//DMSG("TEE: 0x%x-0x%x-0x%x-0x%x-0x%x-0x%x\n", tee_pt.ttbcr, tee_pt.ttbr1, tee_pt.ttbr0, tee_pt.sctlr, tee_pt.sp, tee_pt.vbar);
	if(is_sm_lock_down()) {
		// Verify that the caller comes from TEE kernel and TEE SP sits outside TSEE
		if(((!(tee_pt.ttbcr == 0 && tee_pt.ttbr0 == tsee_init.kpgtbl)) &&
			(!(tee_pt.ttbcr != 0 && tee_pt.ttbr1 == tsee_init.kpgtbl))) ||
		   (tee_pt.sp >= CFG_TSEE_LOAD_ADDR && tee_pt.sp <= CFG_TSEE_LOAD_ADDR + CFG_TSEE_RAM_SIZE)) {
			EMSG("Invalid caller: (ttbcr-ttbr1-ttbr0-sctlr-sp) 0x%x-0x%x-0x%x-0x%x-0x%x\n", 
				 tee_pt.ttbcr, tee_pt.ttbr1, tee_pt.ttbr0, tee_pt.sctlr, tee_pt.sp);
			return 1;
		}
	}
	switch(cmd) {
		case TSEE_RTKS_INIT:
			DMSG("tsee rtks init test call\n");
			break;
		case TSEE_RTKS_DEF_INIT:
			DMSG("tsee rtks definit done\n");
			sm_tee_deinit();
			break;

		//  Page table management

		case TSEE_RTKS_PGD_SET:
			// currently not used
			DMSG("tsee rtks pgd set\n");
			if(!is_kpgtbl_range(arg0) && !is_kpt_range(arg0)) {
				EMSG("Invalid pde pointer for pde write\n");
				break;
			}
			*(uint32_t*)arg0 = arg1;
			dsb(ishst);
			isb();
			break;
		case TSEE_RTKS_PMD_SET:
			//DMSG("tsee rtks pmd set: 0x%x-0x%x\n", arg0, arg1);
			// pde either comes from kpgtbl or kpt
			if(!is_kpgtbl_range(arg0) && !is_kpt_range(arg0)) {
				EMSG("Invalid pde pointer for pde write\n");
				break;
			}
			// pde value should be within kpt ranges
			tmp = arg1 & (~(1 << 10));
			if(!is_kpt_range(tmp)) {
				EMSG("Invalid pde value for pde write\n");
				break;
			}
			if(is_kpgtbl_range(arg0)) {
				*(uint32_t*)arg0 = arg1;
			}else {
				*(uint32_t*)arg0 = arg1;
			}
			dsb(ishst);
			isb();
			break;
		case TSEE_RTKS_PTE_SET:
			//DMSG("tsee rtks pte set: 0x%x-0x%x\n", arg0, arg1);
			// pte pointer could only comes from initial l2pgtbl or kpt
			if(!is_l2pgtbl_range(arg0) && !is_kpt_range(arg0)) {
				EMSG("Invalid pte pointer for pte write: 0x%x-0x%x\n", arg0, arg1);
				break;
			}

			tmp = arg1 & (~(1 << 12));
			// check the target mapping address
			//we have tracked whether it maps the TEE runtime data and prevent it
			if(tsee_is_mempg_protected(tmp)) {
				EMSG("0x%x is protected from mapping\n", arg1);
				break;
			}
			if(is_tee_range(tmp)) {
				if(is_kpt_range(tmp)) {
					EMSG("stop trying to map kpt range 0x%x\n", tmp);
					break;
				}
				if(is_kpgtbl_range(tmp)) {
					EMSG("stop trying to map kpgtbl range 0x%x\n", tmp);
					break;
				}
				if(is_l2pgtbl_range(tmp)) {
					EMSG("stop trying to map l2pgtbl range 0x%x\n", tmp);
					break;
				}
				if(is_kdata_range(tmp) || is_krodata_range(tmp) || is_ktext_range(tmp)) {
					EMSG("stop trying to map TEE text and data region 0x%x\n", tmp);
					break;
				}
				
				
			}else if(is_reeshr_range(tmp)) {
				//TODO: add policy for share memory access
				//DMSG("warning:mapping normal memory\n");
			}else if(is_tsee_range(tmp)) {
				EMSG("stop trying to map tsee range 0x%x\n", tmp);
				break;
			}else if(is_devmem_range(tmp)) {
				DMSG("warning:mapping device memory 0x%x\n", tmp);
			}else if(tmp != 0){
				//TODO: add policy for ree memory access
				// currently allow it
			}
			// OK, check whether it is a kernel or ta mapping
			if(is_l2pgtbl_range(arg0)) {
				*(uint32_t*)arg0 = arg1;
			}else {
				*(uint32_t*)arg0 = arg1;
			}
			dsb(ishst);
			isb();
			break;
		case TSEE_RTKS_PGD_FREE:
			//DMSG("tsee rtks pgd free\n");
			break;
		case TSEE_RTKS_PGD_NEW:
			//DMSG("tsee rtks pgd new: 0x%x\n", arg0);
			if(arg0 >= tsee_init.kpt_start && 
			   arg0 < tsee_init.kpt_end) {
				memset(arg0, 0, PT_SZ);
			}else if(arg0 >= tsee_init.kpt_start2 && 
			   arg0 < tsee_init.kpt_end2) {
				memset(arg0, 0, PT_SZ);
			}else {
				EMSG("Invalid pgd new\n");
				break;
			}
			break;
		case TSEE_RTKS_KRODATA_SET:
			if(is_sm_lock_down()) {
				return 1;
			}
			if(!(arg0 + arg1 <= CFG_TSEE_LOAD_ADDR) && 
			   !(arg0 >= CFG_TSEE_LOAD_ADDR + CFG_TSEE_RAM_SIZE)) {
				EMSG("Invalid krodata range\n");
				return 1;
			}
			DMSG("tsee rtks krodata set: 0x%x-0x%x\n", arg0, arg0 + arg1);
			tsee_init.krodata_start = arg0;
			tsee_init.krodata_end = arg0 + arg1;
			break;
		case TSEE_RTKS_KDATA_SET:
			if(is_sm_lock_down()) {
				return 1;
			}
			if(!(arg0 + arg1 <= CFG_TSEE_LOAD_ADDR) && 
			   !(arg0 >= CFG_TSEE_LOAD_ADDR + CFG_TSEE_RAM_SIZE)) {
				EMSG("Invalid kdata range\n");
				return 1;
			}
			if(tsee_init.kdata_start == 0 && tsee_init.kdata_end  == 0) {
				tsee_init.kdata_start = arg0;
				tsee_init.kdata_end = arg0 + arg1;
			}else if(tsee_init.kdata_start2 == 0 && tsee_init.kdata_end2 == 0) {
				tsee_init.kdata_start2 = arg0;
				tsee_init.kdata_end2 = arg0 + arg1;
			}else {
				EMSG("kdata list is full\n");
			}
			DMSG("tsee rtks kdata set: 0x%x-0x%x\n", arg0, arg0 + arg1);
			break;
		case TSEE_RTKS_KTEXT_SET:
			if(is_sm_lock_down()) {
				return 1;
			}
			if(!(arg0 + arg1 <= CFG_TSEE_LOAD_ADDR) && 
			   !(arg0 >= CFG_TSEE_LOAD_ADDR + CFG_TSEE_RAM_SIZE)) {
				EMSG("Invalid ktext range\n");
				return 1;
			}
			if(tsee_init.ktext_start == 0 && tsee_init.ktext_end  == 0) {
				tsee_init.ktext_start = arg0;
				tsee_init.ktext_end = arg0 + arg1;
			}else if(tsee_init.ktext_start2 == 0 && tsee_init.ktext_end2 == 0) {
				tsee_init.ktext_start2 = arg0;
				tsee_init.ktext_end2 = arg0 + arg1;
			}else {
				EMSG("ktext list is full\n");
			}
			DMSG("tsee rtks ktext set: 0x%x-0x%x\n", arg0, arg0 + arg1);
			break;
		case TSEE_RTKS_KPGTBL_SET:
			if(is_sm_lock_down()) {
				return 1;
			}
			if(!(arg0 + arg1 <= CFG_TSEE_LOAD_ADDR) && 
			   !(arg0 >= CFG_TSEE_LOAD_ADDR + CFG_TSEE_RAM_SIZE)) {
				EMSG("Invalid kpgtbl range\n");
				return 1;
			}
			DMSG("tsee rtks kpgtbl set: 0x%x-0x%x\n", arg0, arg0 + arg1);
			tsee_init.kpgtbl = arg0;
			tsee_init.kpgtbl_size = arg1;
			break;
		case TSEE_RTKS_L2PGTBL_SET:
			if(is_sm_lock_down()) {
				return 1;
			}
			if(!(arg0 + arg1 <= CFG_TSEE_LOAD_ADDR) && 
			   !(arg0 >= CFG_TSEE_LOAD_ADDR + CFG_TSEE_RAM_SIZE)) {
				EMSG("Invalid l2pgtbl range\n");
				return 1;
			}
			DMSG("tsee rtks l2pgtbl set: 0x%x-0x%x\n", arg0, arg0 + arg1);
			tsee_init.l2pgtbl_start = arg0;
			tsee_init.l2pgtbl_end = arg0 + arg1;
			break;
		case TSEE_RTKS_REEMEM_SHR_SET:
			if(is_sm_lock_down()) {
				return 1;
			}
			if(!(arg0 + arg1 <= CFG_TSEE_LOAD_ADDR) && 
			   !(arg0 >= CFG_TSEE_LOAD_ADDR + CFG_TSEE_RAM_SIZE)) {
				EMSG("Invalid reemem share range\n");
				return 1;
			}
			DMSG("tsee rtks reemem share set to: 0x%x-0x%x\n", arg0, arg1 + arg0);
			tsee_init.reemem_shr_start = arg0;
			tsee_init.reemem_shr_end = arg0 + arg1;
			break;
		case TSEE_RTKS_PTS_ADD:
			if(is_sm_lock_down()) {
				return 1;
			}
			if(!(arg0 + arg1 <= CFG_TSEE_LOAD_ADDR) && 
			   !(arg0 >= CFG_TSEE_LOAD_ADDR + CFG_TSEE_RAM_SIZE)) {
				EMSG("Invalid pts range\n");
				return 1;
			}
			DMSG("tsee rtks pts set: 0x%x-0x%x\n", arg0, arg0 + arg1);
			if(tsee_init.kpt_start == 0 && tsee_init.kpt_end  == 0) {
				tsee_init.kpt_start = arg0;
				tsee_init.kpt_end = arg0 + arg1;
			}else if(tsee_init.kpt_start2 == 0 && tsee_init.kpt_end2 == 0) {
				tsee_init.kpt_start2 = arg0;
				tsee_init.kpt_end2 = arg0 + arg1;
			}else {
				EMSG("kpt list is full\n");
			}
			break;
		case TSEE_RTKS_PTS_FREE:
			DMSG("tsee rtks pts free\n");
			break;
		case TSEE_RTKS_TEEMEM_SET:
			if(is_sm_lock_down()) {
				return 1;
			}
			if(!(arg0 + arg1 <= CFG_TSEE_LOAD_ADDR) && 
			   !(arg0 >= CFG_TSEE_LOAD_ADDR + CFG_TSEE_RAM_SIZE)) {
				EMSG("Invalid tee mem range\n");
				return 1;
			}
			DMSG("tsee rtks teemem set to: 0x%x-0x%x\n", arg0, arg1 + arg0);
			if(tsee_init.kmem_start == 0 && tsee_init.kmem_end == 0) {
				tsee_init.kmem_start = arg0;
				tsee_init.kmem_end = arg0 + arg1;
			}else if(tsee_init.kmem_start2 == 0 && tsee_init.kmem_end2 == 0) {
				tsee_init.kmem_start2 = arg0;
				tsee_init.kmem_end2 = arg0 + arg1;
			}else {
				EMSG("kmem list is full\n");
			}
			break;

		// TEE privileged ops	

		case TSEE_RTKS_LOCK_DOWN:
			DMSG("tsee rtks lock down\n");
			sm_lock_down();
			break;
		case TSEE_RTKS_MVBAR_SET:
			DMSG("tsee rtks set mvbar:0x%x\n", arg0);
			sm_write_mvbar((void*)arg0);
			break;
		case TSEE_RTKS_TTBCR_SET:
			DMSG("tsee rtks set ttbcr\n");
			//asm("MCR p15, 0, %[v], c2, c0, 2": :[v]"r" (arg0):);
			tee_pt.ttbcr = arg0;
			break;
		case TSEE_RTKS_TTBR0_SET:
			//DMSG("tsee rtks set ttbr0\n");
			if(arg0 >= CFG_TSEE_LOAD_ADDR && arg0 < CFG_TSEE_LOAD_ADDR + CFG_TSEE_RAM_SIZE) {
				EMSG("Invalid ttbr0 value\n");
			}
			//asm("MCR p15, 0, %[v], c2, c0, 0": :[v]"r" (arg0):);
			tee_pt.ttbr0 = arg0;
			break;
		case TSEE_RTKS_TTBR1_SET:
			//DMSG("tsee rtks set ttbr1\n");
			if(arg0 >= CFG_TSEE_LOAD_ADDR && arg0 < CFG_TSEE_LOAD_ADDR + CFG_TSEE_RAM_SIZE) {
				EMSG("Invalid ttbr1 value\n");
			}
			//asm("MCR p15, 0, %[v], c2, c0, 1": :[v]"r" (arg0):);
			tee_pt.ttbr1 = arg0;
			break;
		case TSEE_RTKS_SCTLR_SET:
			//DMSG("tsee rtks set sctlr\n");
			if(is_sm_lock_down() && (arg0 & 0x1 == 0)) {
				EMSG("Disable MMU after lock down is not allowed\n");
				return 1;
			}
			tee_pt.sctlr = arg0;
			break;
		case TSEE_RTKS_DACR_SET:
			DMSG("tsee rtks set dacr: To be implemented\n");
			if(is_sm_lock_down() && arg0 != 0x55555555) {
				EMSG("Domain access is fully/partially disabled!\n");
			}
			break;
		case TSEE_RTKS_MEM_PROTECT:
			// Once a page is protected, any further mapping from TEE will be denied
			//DMSG("tsee rtks mem map protection: 0x%x-0x%x\n", arg0, arg0 + arg1);
			// check that the range is not in non-TEE ranges
			if(((arg0 >= tsee_init.kmem_end) || (arg0 + arg1 <= tsee_init.kmem_start)) &&
			   ((arg0 >= tsee_init.kmem_end2) || (arg0 + arg1 <= tsee_init.kmem_start2)) &&
			   ((arg0 >= tsee_init.reemem_shr_end) || (arg0 + arg1 <= tsee_init.reemem_shr_start))) {
				EMSG("illegal parameters for TEE mem protection: 0x%x-0x%x \n", arg0, arg0 + arg1);
				break;
			}
			for(tmp = arg0; tmp < arg0 + arg1; tmp += PAGE_SIZE) {
				if(tsee_mempg_protect(tmp) != 0) {
					EMSG("protect 0x%x failed\n", tmp);
				}
			}
			break;
		case TSEE_RTKS_MEM_UNPROTECT:
			//DMSG("tsee rtks mem map unprotection: 0x%x-0x%x\n", arg0, arg0 + arg1);
			// check that the range is not in non-TEE ranges
			if(((arg0 >= tsee_init.kmem_end) || (arg0 + arg1 <= tsee_init.kmem_start)) &&
			   ((arg0 >= tsee_init.kmem_end2) || (arg0 + arg1 <= tsee_init.kmem_start2)) && 
			   ((arg0 >= tsee_init.reemem_shr_end) || (arg0 + arg1 <= tsee_init.reemem_shr_start))) {
				EMSG("illegal parameters for TEE mem unprotection: 0x%x-0x%x \n", arg0, arg0 + arg1);
				break;
			}
			for(tmp = arg0; tmp < arg0 + arg1; tmp += PAGE_SIZE) {
				if(tsee_mempg_unprotect(tmp) != 0) {
					EMSG("unprotect 0x%x failed\n", tmp);
				}
			}
			break;
		default:
			DMSG("TSEE Error: unknown cmd for tsee: 0x%x-0x%x-0x%x\n", cmd, arg0, arg1);
			break;
	}
	return 0;
}
