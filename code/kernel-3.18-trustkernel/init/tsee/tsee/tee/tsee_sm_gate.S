#include <platform_config.h>
#include <asm.S>

.align 5
.global trap_entry
trap_entry:
	b   .
	b   .   /* Undef */
	b   .   /* Syscall */
	b   trap_iabort /* Prefetch abort */
	b   trap_dabort /* Data abort */
	b   .   /* Reserved */
	b   .   /* IRQ */
	b   .   /* FIQ */

trap_iabort:
	ldr		sp, =mon_stktop_others
	SUB     r14, r14, #4            @  lr: instruction causing the abort
	STMFD   r13!, {r0-r12, r14}
	MRS     r2, spsr                @  copy spsr to r2
	STMFD   r13!, {r2}              @  save r2(spsr) to the stack
	STMFD   r13!, {r14}             @  save r14 again (it is not really correct)
	STMFD   r13, {sp, lr}^          @  save user mode sp and lr
	SUB     r13, r13, #8

	MOV     r0, r13                 @  save trapframe as the first parameter
	BL      iabort_handler
	B       .   

trap_dabort:
	ldr		sp, =mon_stktop_others
	SUB     r14, r14, #8            @  lr: instruction causing the abort
	STMFD   r13!, {r0-r12, r14}
	MRS     r2, spsr                @  copy spsr to r2
	STMFD   r13!, {r2}              @  save r2(spsr) to the stack
	STMFD   r13!, {r14}             @  save r14 again (it is not really correct)
	STMFD   r13, {sp, lr}^          @  save user mode sp and lr
	SUB     r13, r13, #8

	MOV     r0, r13                 @  save trapframe as the first parameter
	BL      dabort_handler
	B       .



.section .text.tsee_ro

.macro tsee_entry_gate
############################
	# restore r0-r3
	pop		{lr}
	pop		{r0-r3}
	# re-save spsr
	push	{lr}


	push	{r0-r3}
	
	# Temply save TEE TTBR Regs
	mrc		p15, 0, r1, c12, c0, 0 /* VBAR */
	push	{r1}
	mrc		p15, 0, r1, c1, c0, 0 /* SCTLR */
	push	{r1}
	mov		r2, r1				/* Tmp save sctlr in r2*/
	mrc		p15, 0, r1, c2, c0, 1 /* TTBR1 */
	push	{r1}
	mrc		p15, 0, r1, c2, c0, 0 /* TTBR0 */
	push	{r1}
	mrc		p15, 0, r1, c2, c0, 2 /* TTBCR */
	push	{r1}
	
	# Load TSEE TTBR Regs
	ldr		r0, =tsee_pt
	ldr		r1, [r0], #4
	mcr		p15, 0, r1, c2, c0, 2 /* TTBCR */
	ldr		r1, [r0], #4
	mcr		p15, 0, r1, c2, c0, 0 /* TTBR0 */
	ldr		r1, [r0], #4
	mcr		p15, 0, r1, c2, c0, 1 /* TTBR1 */
	ldr		r1, [r0], #4
	mcr		p15, 0, r1, c1, c0, 0 /* SCTLR */
	ldr		r1, [r0], #4		  /* SP */
	ldr		r1, [r0], #4
	mcr		p15, 0, r1, c12, c0, 0 /* VBAR */

	# To prevent control flow attacks from TEE(e.g., ROP), we need to further check the written TTBR regs
	isb
	ldr		r0, =tsee_pt
	ldr		r1, [r0], #4
	mrc     p15, 0, r3, c2, c0, 2	/* TTBCR */
	cmp		r1, r3
	bne		.
	ldr		r1, [r0], #4
	mrc		p15, 0, r3, c2, c0, 0 /* TTBR0 */
	cmp		r1, r3
	bne		.
	ldr		r1, [r0], #4
	mrc		p15, 0, r3, c2, c0, 1 /* TTBR1 */
	cmp		r1, r3
	bne		.
	ldr		r1, [r0], #4
	mrc		p15, 0, r3, c1, c0, 0 /* SCTLR */
	cmp		r1, r3
	bne		.
	ldr		r1, [r0], #4		  /* SP */
	ldr		r1, [r0], #4
	mrc		p15, 0, r3, c12, c0, 0 /* VBAR */
	cmp		r1, r3
	bne		.

	# Flush TLB 
	mov r0, #0
	mcr 	p15, 0, r0, c7, c5,  0       @ I+BTB cache invalidate
	mcr     p15, 0, r0, c8, c3,  0
	mcr     p15, 0, r0, c7, c5,  6  @ Inval. branch predict. array
	# only flush cache when MMU/dcache/icache/BP of sctlr is different in TEE and TSEE
#if 0
	ldr		r0, =tsee_pt
	ldr		r0, [r0, #12]
	movw	r1, #(1 << 2 | 1 << 12 | 1 << 0 | 1 << 11) 
	and		r0, r0, r1
	and		r2, r2, r1
	cmp		r0, r2
	beq		1f	
	bl		flush_cache_all
1:
#endif
	dsb 
	isb 

	# Securely save states, the values will be checked later
	# BUG/FIXME/TODO: SP_tee is the virtual address of TEE, we need to convert to virtual address of TSEE
	# currently we assume SP_tee has the same virt_addr as phy_addr
	ldr		r0, =tee_pt
	pop		{r1} 				/* TTBCR */
	str		r1, [r0], #4
	pop		{r1}				/* TTBR0 */
	str		r1, [r0], #4
	pop		{r1}				/* TTBR1 */
	str		r1, [r0], #4
	pop		{r1}				/* SCTLR */
	str		r1, [r0], #4		
	pop		{r1}				/* VBAR */
	str		sp, [r0], #4		/* SP, Note that sp has been decreased by 16 */
	str		r1, [r0], #4		/* Save VBAR */


	# Switch to TSEE monitor stack
	pop		{r0-r3}
	ldr		sp, =tsee_pt
	ldr		sp, [sp, #16]
############################
.endm

.macro tsee_exit_gate
############################
	# Save TSEE TTBR Regs
	ldr		r0, =tsee_pt
	mrc		p15, 0, r1, c2, c0, 2 /* TTBCR */
	str		r1, [r0], #4
	mrc		p15, 0, r1, c2, c0, 0 /* TTBR0 */
	str		r1, [r0], #4
	mrc		p15, 0, r1, c2, c0, 1 /* TTBR1 */
	str		r1, [r0], #4
	mrc		p15, 0, r1, c1, c0, 0 /* SCTLR */
	str		r1, [r0], #4
	str		sp, [r0], #4 		  /* SP */
	mrc		p15, 0, r1, c12, c0, 0 /* VBAR */
	str		r1, [r0], #4

	# Load TEE TTBR Regs 
	# To prevent control flow attacks from TEE(e.g., ROP), we need to further check the written TTBR regs
	ldr		r0, =tee_pt
	ldr		r1, [r0], #4
	mcr		p15, 0, r1, c2, c0, 2 /* TTBCR */

	# check ttbcr
	ldr		r1, =tee_pt
	ldr		r1, [r1]
	mrc     p15, 0, r2, c2, c0, 2	/* TTBCR */
	cmp		r1, r2
	bne		.

	ldr		r1, [r0], #4
	mcr		p15, 0, r1, c2, c0, 0 /* TTBR0 */

	# check ttbr0
	ldr		r1, =tee_pt
	ldr		r1, [r1, #4]
	mrc		p15, 0, r2, c2, c0, 0 /* TTBR0 */
	cmp		r1, r2
	bne		.

	ldr		r1, [r0], #4
	mcr		p15, 0, r1, c2, c0, 1 /* TTBR1 */

	# check ttbr1
	ldr		r1, =tee_pt
	ldr		r1, [r1, #8]
	mrc		p15, 0, r2, c2, c0, 1 /* TTBR1 */
	cmp		r1, r2
	bne		.

	ldr		r1, [r0], #4
	mcr		p15, 0, r1, c1, c0, 0 /* SCTLR */

	# check sctlr
	ldr		r1, =tee_pt
	ldr		r1, [r1, #12]
	mrc		p15, 0, r2, c1, c0, 0 /* SCTLR */
	cmp		r1, r2
	bne		.

	ldr		sp, [r0], #4 		  /* SP  */

	# check sp
	ldr		r1, =tee_pt
	ldr		r1, [r1, #16]
	cmp		r1, sp
	bne		.

	ldr		r1, [r0], #4
	mcr		p15, 0, r1, c12, c0, 0 /* VBAR */

	# check vbar
	ldr		r1, =tee_pt
	ldr		r1, [r1, #20]
	mrc		p15, 0, r2, c12, c0, 0 /* VBAR */
	cmp		r1, r2
	bne		.

	# Flush TLB
	mov r0, #0
	mcr 	p15, 0, r0, c7, c5,  0       @ I+BTB cache invalidate
	mcr     p15, 0, r0, c8, c3,  0
	mcr     p15, 0, r0, c7, c5,  6  @ Inval. branch predict. array
	# only flush cache when MMU/dcache/icache/BP of sctlr is different in TEE and TSEE
#if 0
	ldr		r0, =tsee_pt
	ldr		r0, [r0, #12]
	movw	r2, #(1 << 2 | 1 << 12 | 1 << 0 | 1 << 11) 
	and		r0, r0, r2
	ldr		r1, =tee_pt
	ldr		r1, [r1, #12]
	and		r1, r1, r2
	cmp		r0, r1
	beq		2f	
	bl		flush_cache_all
2:
#endif
	dsb 
	isb 

	pop		{r0-r3} // sp has been decreased by 16, so increase it first

############################
.endm

.macro save_callstate
	# save states in montor stack: lr, r0-r3, spsr
	push	{lr}
	push	{r0-r3}
	mrs     r1, spsr
	push	{r1}
.endm

.macro restore_callstate
	# pop spsr, lr
	pop		{r0}
	msr     spsr_cxsf, r0
	pop		{lr}
	movs	pc, lr
.endm

# secure monitor exception vectors 
.align 5
.global monitor_entry 
monitor_entry:
	b   sm_tee_reset_handler  @ reset       
	b   sm_tee_undef_handler  @ undef
	b   monitor_smc			  @ smc
	b   sm_tee_pref_handler   @ pref
	b   sm_tee_abort_handler  @ abort
	b   sm_tee_reserv_handler @ reserv
	b   sm_tee_irq_handler	  @ irq
	b   sm_tee_fiq_handler	  @ fiq

monitor_smc:
	save_callstate
	
	# Clear local monitor
	clrex 
	bl		is_tsee_call
	cmp		r0, #1
	bne		tsee_sm_proxy

	# check where is the calling from
	mrc     p15, 0, r1, c1, c1, 0  // Read Secure Configuration Register 
	tst 	r1, #0x1
	bne		tsee_sm_from_ns_entry
	beq		tsee_sm_from_s_entry
tsee_sm_from_ns_entry:
	# clear NS bit
	push	{r1}
	mrc     p15, 0, r1, c1, c1, 0  // Read Secure Configuration Register 
	bic		r1, #1
	mcr		p15, 0, r1, c1, c1, 0
	pop		{r1}
	tsee_entry_gate
	bl		tsee_rtks_ns_call
	tsee_exit_gate
	# reset NS bit
	push	{r1}
	mrc     p15, 0, r1, c1, c1, 0  // Read Secure Configuration Register 
	orr		r1, r1, #1
	mcr		p15, 0, r1, c1, c1, 0
	pop		{r1}
	restore_callstate
tsee_sm_from_s_entry:
	tsee_entry_gate
	bl		tsee_rtks_call
	tsee_exit_gate
	restore_callstate
tsee_sm_proxy:
	# pop spsr, r0-r3, lr
	pop		{lr}
	pop		{r0-r3}
	pop		{lr}
	b		sm_tee_smc_handler


.global default_sm_handler
.global sm_tee_reset_handler
.global sm_tee_undef_handler
.global sm_tee_smc_handler
.global sm_tee_pref_handler
.global sm_tee_abort_handler
.global sm_tee_reserv_handler
.global sm_tee_irq_handler
.global sm_tee_fiq_handler
default_sm_handler:
sm_tee_reset_handler:
	ldr	pc, [pc, #-8]
sm_tee_undef_handler:
	b		.
sm_tee_smc_handler:
	b		.
sm_tee_pref_handler:
	b		.
sm_tee_abort_handler:
	b		.
sm_tee_reserv_handler:
	b		.
sm_tee_irq_handler:
	b		.
sm_tee_fiq_handler:
	b		.


