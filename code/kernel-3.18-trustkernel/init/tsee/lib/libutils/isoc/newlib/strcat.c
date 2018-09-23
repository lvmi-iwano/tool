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
strcat(char *s1, const char* s2)
{
	char *save = s1;
	for (; *s1; ++s1);
	while((*s1++ = *s2++) != 0);
	return (save);
}

