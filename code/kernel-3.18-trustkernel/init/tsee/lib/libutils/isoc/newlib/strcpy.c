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

char*
strcpy(char *s, const char *t)
{  
	char *os;

	os = s;
	while((*s++ = *t++) != 0)
		;
	return os;
}  
char* strncpy(char *s, const char *t, int n)
{
	char *os;

	os = s;

	while(n-- > 0 && (*s++ = *t++) != 0)
		;

	while(n-- > 0) {
		*s++ = 0;
	}

	return os;
}   


