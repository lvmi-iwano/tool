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

#include <stdlib.h>
#include <string.h>

char *strndup(const char *s, size_t n)
{
	size_t l = strnlen(s, n) + 1;
	char *p = malloc(l);

	if (p) {
		memcpy(p, s, l - 1);
		p[l - 1] = '\0';
	}
	return p;
}
