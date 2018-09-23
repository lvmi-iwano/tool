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
#include <tsee_sm.h>
#include <trace.h>
#include <tee/tsee/tsee_entry.h>
#include <tee/tsee/defs.h>
#include <tee/tsee/tsee_mm.h>
#include <platform_config.h>

// This handlers are put in text section so that monitor_entry could load them 
// without polluting the registers.
// The security is forced by lock down: after lock down the text section is 
// set to read only.
extern void* sm_tee_reset_handler  ;
extern void* sm_tee_undef_handler  ;
extern void* sm_tee_smc_handler 	;
extern void* sm_tee_pref_handler 	;
extern void* sm_tee_abort_handler 	;
extern void* sm_tee_reserv_handler ;
extern void* sm_tee_irq_handler 	;
extern void* sm_tee_fiq_handler 	;

extern void flush_tlb_all();
extern void flush_cache_all();

static void set_jump_addr(uint32_t *instr, uint32_t addr)
{
	uint32_t instr_addr = (uint32_t) instr;
	uint32_t imm = ((uint32_t) (((int32_t) addr - (int32_t) instr_addr - 8) >> 2)) & 0xffffffU;
	*instr = 0xea000000 | imm;
}

static int is_lock_down = 0;
static int is_init = 0;

void mem_enforce_perm(uint32_t start, uint32_t end, uint32_t apx, uint32_t ap, uint32_t xn, uint32_t pxn) {
	int addr = 0;
	uint32_t* pde;
	uint32_t* pte;
	uint32_t* pgtab;
	uint32_t pde_phy;

	if(end <= start)
		return;

	//DMSG("enforcing mem perm:0x%x-0x%x\n", start, end);

	for(addr = start; addr < end;) {
		// find the pde entry
		pde = &((uint32_t*)tsee_init.kpgtbl)[PDE_IDX(addr)];
		if(!(*pde & PE_TYPES)) {
			EMSG("Error:Not page type for addr: 0x%x, val:0x%x\n", addr, *pde);
			return;
		}
		if(*pde & PDE_PAGE_TYPE) {
			pgtab = (uint32_t*)PT_ADDR(*pde);
			pte = &pgtab[PTE_IDX(addr)];
			if(!(*pte & PE_TYPES)) {
				EMSG("Error: unsupported pte type, addr: 0x%x-*pte: 0x%x-*pde:0x%x\n", addr, *pte, *pde);
				return;
			}
			//DMSG("page addr for page va: pa:0x%x, va:0x%x, pte: 0x%x\n", addr, tee_phys_to_virt(addr), *pte);
			*pte &= ~(1 << 9 | 0x3 << 4 | 1 << 0);
			*pte |= (apx << 9 | ap << 4 | xn << 0);
			addr += PAGE_SIZE;
		}else if(*pde & PDE_SECTION_TYPE) {
			//DMSG("page addr for section va: pa:0x%x, va:0x%x, pde: 0x%x\n", addr, 
			//	 tee_phys_to_virt(addr), *pde);
			*pde &= ~(1 << 15 | 0x3 << 10 | 1 << 0 | 1 << 4);
			*pde |= (apx << 15 | ap << 10 | pxn << 0 | xn << 4);
			addr += SECTION_SIZE;
			//DMSG("pde is set to: 0x%x\n", *pde);
		}else {
			EMSG("Error: unsupported pde type\n");
			return;
		}
	}
}

extern uint32_t __tsee_text_ro_start;
extern uint32_t __tsee_text_ro_end;
extern uint32_t __tsee_data_ro_start;
extern uint32_t __tsee_data_ro_end;
extern uint32_t tee_rw_stack_start;
extern uint32_t tee_rw_stack_end;

int is_sm_lock_down(void) {
	return is_lock_down;
}

void sm_lock_down(void) {
	is_lock_down = 1;

	//// memory permission enforcement

	// RODATA and DATA is set to non-executable
	DMSG("enforcing rodata ro nx");
	mem_enforce_perm(tsee_init.krodata_start, tsee_init.krodata_end, 0x0, 0x1, 0x1, 0x0);
	mem_enforce_perm(tee_phys_to_virt(tsee_init.krodata_start), tee_phys_to_virt(tsee_init.krodata_end), 0x0, 0x1, 0x1, 0x0);
	DMSG("enforcing data rw nx");
	mem_enforce_perm(tee_phys_to_virt(tsee_init.kdata_start), tee_phys_to_virt(tsee_init.kdata_end), 0x0, 0x1, 0x1, 0x0);
	mem_enforce_perm(tsee_init.kdata_start, tsee_init.kdata_end, 0x0, 0x1, 0x1, 0x0);
	mem_enforce_perm(tee_phys_to_virt(tsee_init.kdata_start2), tee_phys_to_virt(tsee_init.kdata_end2), 0x0, 0x1, 0x1, 0x0);
	mem_enforce_perm(tsee_init.kdata_start2, tsee_init.kdata_end2, 0x0, 0x1, 0x1, 0x0);


	// TEXT and PGTBL is set to read-only
	DMSG("enforcing ktext ro");
	mem_enforce_perm(tsee_init.ktext_start, tsee_init.ktext_end, 0x1, 0x1, 0x0, 0x0);
	mem_enforce_perm(tee_phys_to_virt(tsee_init.ktext_start), tee_phys_to_virt(tsee_init.ktext_end), 0x1, 0x1, 0x0, 0x0);
	mem_enforce_perm(tsee_init.ktext_start2, tsee_init.ktext_end2, 0x1, 0x1, 0x0, 0x0);
	mem_enforce_perm(tee_phys_to_virt(tsee_init.ktext_start2), tee_phys_to_virt(tsee_init.ktext_end2), 0x1, 0x1, 0x0, 0x0);
	DMSG("enforcing l2pgtbl ro");
	mem_enforce_perm(tsee_init.l2pgtbl_start, tsee_init.l2pgtbl_end, 0x1, 0x1, 0x0, 0x0);
	mem_enforce_perm(tee_phys_to_virt(tsee_init.l2pgtbl_start), tee_phys_to_virt(tsee_init.l2pgtbl_end), 0x1, 0x1, 0x0, 0x0);
	DMSG("enforcing kpt ro");
	mem_enforce_perm(tsee_init.kpt_start, tsee_init.kpt_end, 0x1, 0x1, 0x0, 0x0);
	mem_enforce_perm(tee_phys_to_virt(tsee_init.kpt_start), tee_phys_to_virt(tsee_init.kpt_end), 0x1, 0x1, 0x0, 0x0);
	mem_enforce_perm(tsee_init.kpt_start2, tsee_init.kpt_end2, 0x1, 0x1, 0x0, 0x0);
	mem_enforce_perm(tee_phys_to_virt(tsee_init.kpt_start2), tee_phys_to_virt(tsee_init.kpt_end2), 0x1, 0x1, 0x0, 0x0);
	DMSG("enforcing pgtbl ro nx");
	mem_enforce_perm(tsee_init.kpgtbl, tsee_init.kpgtbl + tsee_init.kpgtbl_size, 0x1, 0x1, 0x1, 0x0);
	mem_enforce_perm(tee_phys_to_virt(tsee_init.kpgtbl), tee_phys_to_virt(tsee_init.kpgtbl + tsee_init.kpgtbl_size), 0x1, 0x1, 0x1, 0x0);

	// TSEE Range is set to read-only and non-accessible
	DMSG("enforcing tsee ro and non-accessible");
	mem_enforce_perm(CFG_TSEE_LOAD_ADDR, CFG_TSEE_LOAD_ADDR + CFG_TSEE_RAM_SIZE, 0x0, 0x0, 0x0, 0x0);
	mem_enforce_perm((uint32_t)&__tsee_text_ro_start, (uint32_t)&__tsee_text_ro_end, 0x1, 0x1, 0x0, 0x0);
	mem_enforce_perm((uint32_t)&__tsee_data_ro_start, (uint32_t)&__tsee_data_ro_end, 0x1, 0x1, 0x1, 0x0);
	// enable shadow monitor stack rw
	mem_enforce_perm((uint32_t)&tee_rw_stack_start, (uint32_t)&tee_rw_stack_end, 0x0, 0x1, 0x1, 0x0);

	// lock down TSEE gates TLB entry
	tlb_lockdown((uint32_t)&__tsee_text_ro_start);
	tlb_lockdown((uint32_t)&__tsee_data_ro_start);
	tlb_lockdown((uint32_t)&tee_rw_stack_start);
	DMSG("tsee and tee has been lock-down \n");

}
void sm_tee_deinit(void) {
	is_init = 1;
	
}

void sm_write_mvbar(void* handler) {
	if(is_init) {
		EMSG("Error: TEE has de-init, deny write to mvbar\n");
		return;
	}
	set_jump_addr((uint32_t*)&sm_tee_reset_handler, handler);
	set_jump_addr((uint32_t*)&sm_tee_undef_handler, (char*)handler + 4);
	set_jump_addr((uint32_t*)&sm_tee_smc_handler, 	(char*)handler + 8);
	set_jump_addr((uint32_t*)&sm_tee_pref_handler, 	(char*)handler + 12);
	set_jump_addr((uint32_t*)&sm_tee_abort_handler, (char*)handler + 16);
	set_jump_addr((uint32_t*)&sm_tee_reserv_handler,(char*)handler + 20);
	set_jump_addr((uint32_t*)&sm_tee_irq_handler, 	(char*)handler + 24);
	set_jump_addr((uint32_t*)&sm_tee_fiq_handler, 	(char*)handler + 28);
	DMSG("sm handler is set to: 0x%x-0x%x\n", handler, *(uint32_t*)&sm_tee_reset_handler);
}

// DO NOT print any information here because it is in tsee_ro region
int is_tsee_call(uint32_t prefix) {
	if((prefix & (~0xff)) == TSEE_RTKS_PREFIX) {
		//DMSG("TSEE CALL");
		return 1;
	}
	//DMSG("Non-TSEE CALL");
	return 0;
}

pt_state_t tsee_pt __attribute__((section(".data.tsee_ro"))) = { .ttbcr = 0, .ttbr0 = 0, .ttbr1 = 0, .sctlr = 0};
pt_state_t tee_pt  __attribute__((section(".data.tsee_ro"))) = { .ttbcr = 0, .ttbr0 = 0, .ttbr1 = 0, .sctlr = 0};

// map TSEE into TEE, called when entering TSEE from TEE
void map_tsee(void) {
	//DMSG("enable access to TSEE");
	// save TEE states
    asm("MRC p15, 0, %[r], c2, c0, 2": [r]"=r" (tee_pt.ttbcr)::);
    asm("MRC p15, 0, %[r], c2, c0, 0": [r]"=r" (tee_pt.ttbr0)::);
    asm("MRC p15, 0, %[r], c2, c0, 1": [r]"=r" (tee_pt.ttbr1)::);
    asm("MRC p15, 0, %[r], c1, c0, 0": [r]"=r" (tee_pt.sctlr)::);

	// restore TSEE states
    asm("MCR p15, 0, %[v], c2, c0, 2": :[v]"r" (tsee_pt.ttbcr):);
    asm("MCR p15, 0, %[v], c2, c0, 1": :[v]"r" (tsee_pt.ttbr1):);
    asm("MCR p15, 0, %[v], c2, c0, 0": :[v]"r" (tsee_pt.ttbr0):);
	asm("MCR p15, 0, %[r], c1, c0, 0": :[r]"r" (tsee_pt.sctlr):);

	//DMSG("TSEE: 0x%x-0x%x-0x%x-0x%x\n", tsee_pt.ttbcr, tsee_pt.ttbr1, tsee_pt.ttbr0, tsee_pt.sctlr);

	flush_cache_all();
	flush_tlb_all();
}

// remove TSEE mapping from TEE, called when returning TEE from TSEE
void unmap_tsee(void) {
	//DMSG("disable access to TSEE");
	// save TSEE states
	asm("MRC p15, 0, %[r], c2, c0, 2": [r]"=r" (tsee_pt.ttbcr)::);
    asm("MRC p15, 0, %[r], c2, c0, 0": [r]"=r" (tsee_pt.ttbr0)::);
    asm("MRC p15, 0, %[r], c2, c0, 1": [r]"=r" (tsee_pt.ttbr1)::);
    asm("MRC p15, 0, %[r], c1, c0, 0": [r]"=r" (tsee_pt.sctlr)::);

	// restore TEE states
    asm("MCR p15, 0, %[v], c2, c0, 2": :[v]"r" (tee_pt.ttbcr):);
    asm("MCR p15, 0, %[v], c2, c0, 1": :[v]"r" (tee_pt.ttbr1):);
    asm("MCR p15, 0, %[v], c2, c0, 0": :[v]"r" (tee_pt.ttbr0):);
	asm("MCR p15, 0, %[r], c1, c0, 0": :[r]"r" (tee_pt.sctlr):);

	//DMSG("TEE: 0x%x-0x%x-0x%x-0x%x\n", tee_pt.ttbcr, tee_pt.ttbr1, tee_pt.ttbr0, tee_pt.sctlr);

	flush_cache_all();
	flush_tlb_all();
}
