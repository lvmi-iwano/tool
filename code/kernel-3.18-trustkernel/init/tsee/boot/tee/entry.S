#include <platform_config.h>
#include <asm.S>

.section .data
.balign 4

FUNC get_core_pos , : 
	read_mpidr r0
	and r0, r0, #0x3
	bx  lr  
END_FUNC get_core_pos


.section .text.boot
FUNC _start , :
	b	reset
	b	.	/* Undef */
	b	.	/* Syscall */
	b	.	/* Prefetch abort */
	b	.	/* Data abort */
	b	.	/* Reserved */
	b	.	/* IRQ */
	b	.	/* FIQ */
END_FUNC _start

LOCAL_FUNC reset , :
	# check the cpuid 
	mrc     p15, 0, r0, c0, c0, 5   @ Read CPU ID register
	and		r0, r0, #0x3
	cmp     r0, #0                  @ Is this CPU0?
	blne    reset_secondary
	# clear the bss section, the svc stack, and kernel page table
    LDR     r1, =__bss_start
    LDR     r2, =__bss_end
    MOV     r3, #0x00
1:
    CMP     r1, r2
    STMLTIA r1!, {r3}
    BLT     1b
	# install stack for init monitor modes
	MSR     CPSR_cxsf, #(0x16| 0xc0)
    LDR     sp, =mon_stktop_init 
	# prepare stack for tsee 
	LDR		r0, =tsee_pt;
	LDR		r1, =mon_stktop
	STR		r1, [r0, #16]
	# install monitor exception vector 
	LDR		r0,	=monitor_entry
	MCR 	p15,0, r0, c12, c0, 1 
		# init CPU
	BL		cpu_init
	BL		console_init
	LDR		r0, =default_sm_handler
	BL		sm_write_mvbar
	B		start

END_FUNC reset

LOCAL_FUNC reset_secondary , :
boot_secondary:
	# install monitor exception vector for startup
	LDR     r0, =monitor_entry
	MCR     p15,0, r0, c12, c0, 1 
	# initialize stack pointers for monitor mode
	MSR     CPSR_cxsf, #(0x16|0xc0)
	bl  	get_core_pos
	lsl 	r0, #2
	LDR     r1, =mon_stktop_others 
	LDR		sp, [r1, r0]

	# initialize cpu environment
	BL      cpu_init
	LDR		r0, =default_sm_handler
	BL		sm_write_mvbar
	b       start_secondary
END_FUNC reset_secondary

.global cpu_init
.func cpu_init 
cpu_init:
	push {r1,lr} 
	@ Disable MMU 
	mrc p15, 0, r1, c1, c0, 0       @ Read Configuration Register
	bic r1, r1, #0x1
	mcr p15, 0, r1, c1, c0, 0       @ Write Configuration Register 

	@ Disable L1 Caches 
	mrc p15, 0, r1, c1, c0, 0
	bic r1, r1, #(0x1 << 12)    @ Disable I cache 
	bic r1, r1, #(0x1 << 2)     @ Disable D cache 
	mcr p15, 0, r1, c1, c0, 0

	@ Invalidate Instruction cache 
	mov R1,#0
	mcr p15, 0, r1, c7, c5, 0

	@ Invalidate TLBs 
	mov r1, #0x0
	mcr p15, 0, r1, c8, c3, 0

	mrc     p15, 0, r1, c1, c0, 0        @ Read SCTLR
	bic     r1, r1, #0x10000000
	bic     r1, r1, #0x00002000
	orr     r1, r1, #0x00004000    @bit 14. replacement strategy for cacheds, BTAC and micro TLB: round-robin replacement
	mcr     p15, 0, r1, c1, c0, 0

	mrc		p15, 0, r1, c1, c0, 1        @ Read ACTLR
	orr		r1, r1, #(0x1 << 6)          @ bit 6, SMP 
	orr		r1, r1, #(0x1 << 0)			 @ bit 0, FW, cache and TLB maintainance broadcast
	orr		r1, r1, #(0x1 << 2)          @ bit 2, enable L1 prefetch
	mcr		p15, 0, r1, c1, c0, 1		 @ Write ACTLR

	movw	r1, #0x3fff 				 @ Get NSACR_REG_VAL
	movt	r1, #0x0007
	mcr		p15, 0, r1, c1, c1, 2		 @ Write NSACR

	pop {r1, lr} 
	bx lr
.endfunc

# flush tlb
.global flush_tlb_all
.func flush_tlb_all
flush_tlb_all: 
	mcr     p15, 0, r0, c8, c3,  0
	mcr     p15, 0, r0, c7, c5,  6  @ Inval. branch predict. array
	dsb 
	isb 
	mov pc, lr
.endfunc

# flush cache: ARM uses virtual index cache
.global flush_cache_all
.func flush_cache_all
flush_cache_all:
	stmfd   sp!, {r0-r12, lr}
	bl  v7_flush_dcache_all 
	mov r0, #0
	mcr p15, 0, r0, c7, c5, 0       @ I+BTB cache invalidate
	dsb 
	isb 
	ldmfd   sp!, {r0-r12, lr}
	mov pc, lr
.endfunc

.global v7_flush_dcache_all
.func v7_flush_dcache_all
v7_flush_dcache_all:
	dmb                 @ ensure ordering with previous memory accesses
	mrc p15, 1, r0, c0, c0, 1       @ read clidr
	ands    r3, r0, #0x7000000      @ extract loc from clidr
	mov r3, r3, lsr #23         @ left align loc bit field
	beq finished            @ if loc is 0, then no need to clean
	mov r10, #0             @ start clean at cache level 0
loop1:
	add r2, r10, r10, lsr #1        @ work out 3x current cache level
	mov r1, r0, lsr r2          @ extract cache type bits from clidr
	and r1, r1, #7          @ mask of the bits for current cache only
	cmp r1, #2              @ see what cache we have at this level
	blt skip                @ skip if no cache, or just i-cache
	mcr p15, 2, r10, c0, c0, 0      @ select current cache level in cssr
	isb                 @ isb to sych the new cssr&csidr
	mrc p15, 1, r1, c0, c0, 0       @ read the new csidr
	and r2, r1, #7          @ extract the length of the cache lines
	add r2, r2, #4          @ add 4 (line length offset)
	ldr r4, =0x3ff
	ands    r4, r4, r1, lsr #3      @ find maximum number on the way size
	clz r5, r4              @ find bit position of way size increment
	ldr r7, =0x7fff
	ands    r7, r7, r1, lsr #13     @ extract max number of the index size
loop2:  
	mov r9, r4              @ create working copy of max way size
loop3:
	orr r11, r10, r9, lsl r5        @ factor way and cache number into r11
	orr r11, r11, r7, lsl r2        @ factor index number into r11
	mcr p15, 0, r11, c7, c14, 2     @ clean & invalidate by set/way
	subs    r9, r9, #1          @ decrement the way
	bge loop3
	subs    r7, r7, #1          @ decrement the index
	bge loop2
skip:
	add r10, r10, #2            @ increment cache number
	cmp r3, r10
	bgt loop1
finished:
	mov r10, #0             @ swith back to cache level 0
	mcr p15, 2, r10, c0, c0, 0      @ select current cache level in cssr
	dsb
	isb
	mov pc, lr
.endfunc

/* void tlb_lockdown(uint32_t addr);*/
.global tlb_lockdown
.func tlb_lockdown
tlb_lockdown:
	mov r1, r0         		@ set r1 to the value of the address to be locked down
	mcr p15,0,r1,c8,c7,1    @ invalidate TLB single entry to ensure that Addr is not already in the TLB
	mrc p15,0,r0,c10,c0,0   @ read the lockdown register
	orr r0,r0,#1            @ set the preserve bit
	mcr p15,0,r0,c10,c0,0   @ write to the lockdown register
	ldr r1,[r1]             @ TLB will miss, and entry will be loaded 
	mrc p15,0,r0,c10,c0,0   @ read the lockdown register (victim will have incremented)
	bic r0,r0,#1            @ clear preserve bit
	mcr p15,0,r0,c10,c0,0   @ write to the lockdown register
	mov pc, lr
.endfunc

.section tee_img, "a" 
.incbin "tee.bin";
