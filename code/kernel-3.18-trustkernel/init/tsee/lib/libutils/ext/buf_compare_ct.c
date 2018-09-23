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

#include <string_ext.h>

int buf_compare_ct(const void *s1, const void *s2, size_t n)
{
	int res = 0;
	unsigned char *c1 = (unsigned char *)s1;
	unsigned char *c2 = (unsigned char *)s2;

	while (n--) {
		res |= (*c1 ^ *c2);
		c1++;
		c2++;
	}

	res |= res >> 4;
	res |= res >> 2;
	res |= res >> 1;
	res &= 1;

	return res;
}
