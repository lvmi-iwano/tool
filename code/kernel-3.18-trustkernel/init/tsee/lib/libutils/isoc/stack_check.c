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

void *__stack_chk_guard = (void *)0x00000aff;

void __attribute__((noreturn)) __stack_chk_fail(void);

void __stack_chk_fail(void)
{
	while (1)
		;
}

