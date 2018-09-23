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

char *strdup(const char *s)
{
	size_t l = strlen(s) + 1;
	char *p = malloc(l);

	if (p)
		memcpy(p, s, l);
	return p;
}
