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

#include <stdbool.h>
#include <trace.h>
#include <console.h>

const char trace_ext_prefix[] = "TrustKernel_TSEE";
int trace_level = TRACE_LEVEL;

void trace_ext_puts(const char *str)
{
	const char *p;

	console_flush();
	
	for (p = str; *p; p++) {
		console_putc(*p);
	}

	console_flush();
}

int trace_ext_get_thread_id(void)
{
	return -1;
}
