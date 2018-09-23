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

#include <string.h>

char *
strrchr(const char *p, int ch)
{
	union {
		const char *cp;
		char *p;
	}u;
	char *save;
	
	u.cp = p;
	for (save = 0;; ++u.p) {
		if(*u.p == ch)
			save = u.p;
		if(*u.p == '\0')
			return (save);
	}
}
