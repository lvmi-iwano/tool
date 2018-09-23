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
#include <trace.h>
/**
 * Prefetch abort fault strings
 */
static const char* prefetch_abt_fault_string[] =
{
	"INVALID ENTRY",                            /*  0 = 0b00000, */
	"INVALID ENTRY",                            /*  1 = 0b00001, */
	"Debug Event",                              /*  2 = 0b00010, */
	"Access Flag Fault: Section",               /*  3 = 0b00011, */
	"INVALID ENTRY",                            /*  4 = 0b00100, */
	"Translation Fault: Section",               /*  5 = 0b00101, */
	"Access Flag Fault: Page",                  /*  6 = 0b00110, */
	"Translation Fault: Page",                  /*  7 = 0b00111, */
	"Synchronous External Abort",               /*  8 = 0b01000, */
	"Domain Fault: Section",                    /*  9 = 0b01001, */
	"INVALID ENTRY",                            /*  a = 0b01010, */
	"Domain Fault: Page",                       /*  b = 0b01011, */
	"Translation Table Talk 1st Level sync abt",/*  c = 0b01100, */
	"Permission Fault: Section",                 /*  d = 0b01101, */
	"Translation Table Walk 2nd Level sync abt",/*  e = 0b01110, */
	"Permission Fault: Page",                   /*  f = 0b01111, */
	"INVALID ENTRY",                            /* 10 = 0b10000, */
	"INVALID ENTRY",                            /* 11 = 0b10001, */
	"INVALID ENTRY",                            /* 12 = 0b10010, */
	"INVALID ENTRY",                            /* 13 = 0b10011, */
	"IMPLEMENTATION DEFINED Lockdown",          /* 14 = 0b10100, */
	"INVALID ENTRY",                            /* 15 = 0b10101, */
	"INVALID ENTRY",                            /* 16 = 0b10110, */
	"INVALID ENTRY",                            /* 17 = 0b10111, */
	"INVALID ENTRY",                            /* 18 = 0b11000, */
	"Memory Access Synchronous Parity Error",   /* 19 = 0b11001, */
	"IMPLEMENTATION DEFINED Coprocessor Abort", /* 1a = 0b11010, */
	"INVALID ENTRY",                            /* 1b = 0b11011, */
	"Translation Table Walk 1st Level parity",  /* 1c = 0b11100, */
	"INVALID ENTRY",                            /* 1d = 0b11101, */
	"Translation Table Walk 2nd Level parity",  /* 1e = 0b11110, */
	"INVALID ENTRY",                            /* 1f = 0b11111, */
};

/**
 * Data abort fault strings
 */
static const char* data_abt_fault_string[] =
{
	"INVALID ENTRY",
	"Alignment Fault",
	"Debug Event",
	"Access Flag - Section",
	"Instruction Cache Maintainance Fault",
	"Translation Fault - Section",
	"Access Flag - Page",
	"Translation Fault - Page",
	"Synchronous External Abort",
	"Domain Fault - Section",
	"INVALID ENTRY",
	"Domain Fault - Page",
	"Translation Table Talk 1st Level sync abt",
	"Permission Fault - Section",
	"Translation Table Walk 2nd Level sync abt",
	"Permission Fault - Page",
	"INVALID ENTRY",                            /* 10 = 0b10000, */
	"INVALID ENTRY",                            /* 11 = 0b10001, */
	"INVALID ENTRY",                            /* 12 = 0b10010, */
	"INVALID ENTRY",                            /* 13 = 0b10011, */
	"IMPLEMENTATION DEFINED Lockdown",          /* 14 = 0b10100, */
	"INVALID ENTRY",                            /* 15 = 0b10101, */
	"Asynchronous external abort",              /* 16 = 0b10110, */
	"INVALID ENTRY",                            /* 17 = 0b10111, */
	"Memory Access Synchronous Parity Error",   /* 18 = 0b11000, */
	"Memory Access Synchronous Parity Error-2", /* 19 = 0b11001, */
	"IMPLEMENTATION DEFINED Coprocessor Abort", /* 1a = 0b11010, */
	"INVALID ENTRY",                            /* 1b = 0b11011, */
	"Translation Table Walk 1st Level parity",  /* 1c = 0b11100, */
	"INVALID ENTRY",                            /* 1d = 0b11101, */
	"Translation Table Walk 2nd Level parity",  /* 1e = 0b11110, */
	"INVALID ENTRY",                            /* 1f = 0b11111, */
};

struct trapframe {
	uint32_t 	sp_usr;     // user mode sp
	uint32_t    lr_usr;     // user mode lr
	uint32_t    r14_svc;    // r14_svc (r14_svc == pc if SWI)
	uint32_t    spsr;
	uint32_t    r0; 
	uint32_t    r1; 
	uint32_t    r2; 
	uint32_t    r3; 
	uint32_t    r4; 
	uint32_t    r5; 
	uint32_t    r6; 
	uint32_t    r7; 
	uint32_t    r8; 
	uint32_t    r9; 
	uint32_t    r10;
	uint32_t    r11;
	uint32_t    r12;
	uint32_t    pc;         // (lr on entry) instruction to resume execution
};

void dump_trapframe (struct trapframe *tf);
// trap routine
void dabort_handler (struct trapframe *r)
{
    uint32_t dfs, fa;

    // read data fault status register
    asm("MRC p15, 0, %[r], c5, c0, 0": [r]"=r" (dfs)::);

    // read the fault address register
    asm("MRC p15, 0, %[r], c6, c0, 0": [r]"=r" (fa)::);
    
	uint32_t fault_status = (dfs & 0xF) | ((dfs & 0x400) >> 6);
    EMSG ("data abort: instruction 0x%x, fault addr 0x%x, (reason: 0x%x)  %s\r\n",
             r->pc, fa, dfs,(char*)data_abt_fault_string[fault_status]);

	if(fault_status == 0x16) {
 		return;

	}
	dump_trapframe(r);
	EMSG("TSEE Panic\n");
	while(1);
}

// trap routine
void iabort_handler (struct trapframe *r)
{
    uint32_t ifs;
    
    // read fault status register
    asm("MRC p15, 0, %[r], c5, c0, 0": [r]"=r" (ifs)::);

	uint32_t fault_status = (ifs & 0xF) | ((ifs & 0x400) >> 6);
    EMSG ("prefetch abort at: 0x%x (reason: 0x%x) %s\r\n", r->pc, ifs,(char *)prefetch_abt_fault_string[fault_status]);
	
    dump_trapframe (r);

	EMSG("TSEE Panic\n");
	while(1);
}

void dump_trapframe (struct trapframe *tf)
{
    EMSG ("r14_svc: 0x%x\r\n", tf->r14_svc);
    EMSG ("   spsr: 0x%x\r\n", tf->spsr);
    EMSG ("     r0: 0x%x\r\n", tf->r0);
    EMSG ("     r1: 0x%x\r\n", tf->r1);
    EMSG ("     r2: 0x%x\r\n", tf->r2);
    EMSG ("     r3: 0x%x\r\n", tf->r3);
    EMSG ("     r4: 0x%x\r\n", tf->r4);
    EMSG ("     r5: 0x%x\r\n", tf->r5);
    EMSG ("     r6: 0x%x\r\n", tf->r6);
    EMSG ("     r7: 0x%x\r\n", tf->r7);
    EMSG ("     r8: 0x%x\r\n", tf->r8);
    EMSG ("     r9: 0x%x\r\n", tf->r9);
    EMSG ("    r10: 0x%x\r\n", tf->r10);
    EMSG ("    r11: 0x%x\r\n", tf->r11);
    EMSG ("    r12: 0x%x\r\n", tf->r12);
    EMSG ("     pc: 0x%x\r\n", tf->pc);
    EMSG ("     lr_usr: 0x%x\r\n", tf->lr_usr);
} 

