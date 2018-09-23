	.macro FUNC name colon
	.global \name
	.type \name , %function
	\name \colon
	.endm

	.macro DATA name colon
	.global \name
	.type \name , %object
	\name \colon
	.endm

	.macro LOCAL_FUNC name colon
	.type \name , %function
	\name \colon
	.endm

	.macro LOCAL_DATA name colon
	.type \name , %object
	\name \colon
	.endm

	.macro END_DATA name
	.size \name , .-\name
	.endm

	.macro END_FUNC name
	.size \name , .-\name
	.endm


	/* Please keep them sorted based on the CRn register */
	.macro read_mpidr reg
	mrc	p15, 0, \reg, c0, c0, 5
	.endm

	.macro read_sctlr reg
	mrc	p15, 0, \reg, c1, c0, 0
	.endm

	.macro write_sctlr reg
	mcr	p15, 0, \reg, c1, c0, 0
	.endm

	.macro write_actlr reg
	mcr	p15, 0, \reg, c1, c0, 1
	.endm

	.macro read_scr reg
	mrc	p15, 0, \reg, c1, c1, 0
	.endm

	.macro write_scr reg
	mcr	p15, 0, \reg, c1, c1, 0
	.endm

	.macro write_nsacr reg
	mcr	p15, 0, \reg, c1, c1, 2
	.endm

	.macro write_ttbr0 reg
	mcr	p15, 0, \reg, c2, c0, 0
	.endm

	.macro write_ttbr1 reg
	mcr	p15, 0, \reg, c2, c0, 1
	.endm

	.macro write_ttbcr reg
	mcr	p15, 0, \reg, c2, c0, 2
	.endm

	.macro write_dacr reg
	mcr	p15, 0, \reg, c3, c0, 0
	.endm

	.macro read_dfsr reg
	mrc	p15, 0, \reg, c5, c0, 0
	.endm

	.macro write_iciallu
	/* Invalidate all instruction caches to PoU (register ignored) */
	mcr	p15, 0, r0, c7, c5, 0
	.endm

	.macro write_bpiall
	/* Invalidate entire branch predictor array (register ignored) */
	mcr	p15, 0, r0, c7, c5, 6
	.endm

	.macro write_tlbiall
	/* Invalidate entire unified TLB (register ignored) */
	mcr	p15, 0, r0, c8, c7, 0
	.endm

	.macro write_tlbiallis
	/* Invalidate entire unified TLB Inner Sharable (register ignored) */
	mcr	p15, 0, r0, c8, c3, 0
	.endm

	.macro write_tlbiasidis reg
	/* Invalidate unified TLB by ASID Inner Sharable */
	mcr	p15, 0, \reg, c8, c3, 2
	.endm

	.macro write_vbar reg
	mcr	p15, 0, \reg, c12, c0, 0
	.endm

	.macro write_mvbar reg
	mcr	p15, 0, \reg, c12, c0, 1
	.endm

	.macro write_contextidr reg
	mcr	p15, 0, \reg, c13, c0, 1
	.endm

	.macro read_contextidr reg
	mrc	p15, 0, \reg, c13, c0, 1
	.endm

	.macro write_pcr reg
	mcr  p15, 0, \reg, c15, c0, 0
	.endm

	.macro read_actlr reg
	mrc	p15, 0, \reg, c1, c0, 1
	.endm

	.macro read_nsacr reg
	mrc	p15, 0, \reg, c1, c1, 2
	.endm


