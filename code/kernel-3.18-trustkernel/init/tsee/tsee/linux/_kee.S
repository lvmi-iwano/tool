#include <linux/tsee/thee.h>

#define KEE_ELF_PATH "../../../../../kernel-3.18/init/tsee/tsee/linux/kee.elf"

// int  _kee_entry(uint64_t target, ...)
.global _kee_entry
_kee_entry:
	mrs		x0, DAIF           // read interrupt mask bits
	str		x0, [sp, #-8]!     // save interrupt mask bits
	msr		DAIFset, 0x3	   // mask all interrupts

	mrs		x0, ttbr1_el1	   // read existing ttbr1 value
	str		x0, [sp, #-8]!     // save existing ttbr1 value

	#msr		ttbr1_el1, xzr	   // load the value zero to TTBR1
	isb

	bl	 	tsee_main
	b		_kee_exit


// void _kee_exit()
.global _kee_exit
_kee_exit:
	nop
	nop
	nop
	nop

	msr		DAIFset, 0x3	   // mask all interrupts
	ldr		x2, [sp, #8]!	   // reload kernel ttbr1 value
	dsb		sy
	msr		ttbr1_el1, x2	   // restore ttbr1 to kernel value 

	// isolated page boundry
	isb
	tlbi	vmalle1			   // invalidate the TLB
	isb 

	ldr		x2, [sp, #8]!	   // reload interrupts mask bits
	msr		DAIF, x2		   // restore interrupts mask bits register

	ret



.section .kee, "ax"
.global _skee
_skee:
.incbin KEE_ELF_PATH
.global _ekee
_ekee:
.section .text
