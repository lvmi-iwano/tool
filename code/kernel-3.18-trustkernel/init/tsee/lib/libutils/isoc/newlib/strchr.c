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

char *strchr(const char *s, int c){                                                                                                                     
	char *ptr = (char*)s;                                                                                                                               
	while (*ptr != (char)c) {                                                                                                                           
		if(*ptr == '\0')                                                                                                                                
			return NULL;                                                                                                                                
		ptr++;
	}   
	return ptr;                                                                                                                                         
}  
