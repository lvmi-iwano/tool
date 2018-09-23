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

#ifndef __TSEE_ENTRY_H_
#define __TSEE_ENTRY_H_

#ifndef __ASSEMBLY__

#define TSEE_RTKS_PREFIX  			(0xdeadbe00)
#define TSEE_RTKS_CMDID(__CMD_ID) 	(((__CMD_ID) & 0xff) | TSEE_RTKS_PREFIX)

#define TSEE_RTKS_INIT     			TSEE_RTKS_CMDID(0)
#define TSEE_RTKS_DEF_INIT     		TSEE_RTKS_CMDID(1)

#define TSEE_RTKS_PGD_SET  			TSEE_RTKS_CMDID(0x21)
#define TSEE_RTKS_PMD_SET  			TSEE_RTKS_CMDID(0x22)
#define TSEE_RTKS_PTE_SET  			TSEE_RTKS_CMDID(0x23)
#define TSEE_RTKS_PGD_FREE 			TSEE_RTKS_CMDID(0x24)
#define TSEE_RTKS_PGD_NEW  			TSEE_RTKS_CMDID(0x25)
#define TSEE_RTKS_LOCK_DOWN			TSEE_RTKS_CMDID(0x26)
#define TSEE_RTKS_MVBAR_SET			TSEE_RTKS_CMDID(0x27)

#define TSEE_RTKS_TTBCR_SET			TSEE_RTKS_CMDID(0x29)
#define TSEE_RTKS_TTBR0_SET			TSEE_RTKS_CMDID(0x2a)
#define TSEE_RTKS_TTBR1_SET			TSEE_RTKS_CMDID(0x2b)
#define TSEE_RTKS_SCTLR_SET			TSEE_RTKS_CMDID(0x2c)
#define TSEE_RTKS_DACR_SET			TSEE_RTKS_CMDID(0x2d)

#define TSEE_RTKS_KRODATA_SET		TSEE_RTKS_CMDID(0x30)
#define TSEE_RTKS_KTEXT_SET			TSEE_RTKS_CMDID(0x31)
#define TSEE_RTKS_KDATA_SET			TSEE_RTKS_CMDID(0x32)
#define TSEE_RTKS_KPGTBL_SET		TSEE_RTKS_CMDID(0x33)
#define TSEE_RTKS_REEMEM_SHR_SET	TSEE_RTKS_CMDID(0x34)
#define TSEE_RTKS_PTS_ADD			TSEE_RTKS_CMDID(0x35)
#define TSEE_RTKS_PTS_FREE			TSEE_RTKS_CMDID(0x36)
#define TSEE_RTKS_TEEMEM_SET		TSEE_RTKS_CMDID(0x37)

#define TSEE_RTKS_L2PGTBL_SET		TSEE_RTKS_CMDID(0x39)

#define TSEE_RTKS_MEM_PROTECT		TSEE_RTKS_CMDID(0x40)
#define TSEE_RTKS_MEM_UNPROTECT		TSEE_RTKS_CMDID(0x41)

#define TSEE_RTKS_MEM_PROTECT_NS	TSEE_RTKS_CMDID(0x50)
#define TSEE_RTKS_MEM_UNPROTECT_NS	TSEE_RTKS_CMDID(0x51)

// 4GB memory bitmap: 0x100000000 / 1024 / 1024 / 8
// track protected pages and double mapping pages
#define TSEE_RTKS_PGT_BITMAP_LEN 	0x20000


struct tsee_rtks_init {
	// TEE rodata
	u32	krodata_start;
	u32 krodata_end;
	
	// TEE data
	u32 kdata_start;
	u32 kdata_end;
	u32 kdata_start2;
	u32 kdata_end2;


	// TEE text
	u32 ktext_start;
	u32 ktext_end;
	u32 ktext_start2;
	u32 ktext_end2;

	// TEE kernel pgd
	u32 kpgtbl;
	u32 kpgtbl_size;

	// TEE page tables
	u32 l2pgtbl_start;
	u32 l2pgtbl_end;
	u32 kpt_start;
	u32 kpt_end;
	u32 kpt_start2;
	u32 kpt_end2;

	// page tracking bitmaps
	u32 tsee_rtks_phymap;

	// TEE memory range
	u32 kmem_start;
	u32 kmem_end;
	u32 kmem_start2;
	u32 kmem_end2;

	// REE Shared memory
	u32 reemem_shr_start;
	u32 reemem_shr_end;

} __attribute__((packed));

typedef struct tsee_rtks_init tsee_rtks_init_t;


#endif /* __ASSEMBLY__ */
#endif /* __TSEE_ENTRY_H_ */
