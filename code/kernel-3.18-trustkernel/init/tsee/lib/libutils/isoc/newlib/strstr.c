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

char *strstr(const char *searchee, const char *lookfor){ 
	if (*searchee == 0){ 
		if (*lookfor)
			return (char *) NULL;
		return (char *) searchee;
	}   
	while (*searchee){
		unsigned int i;
		i = 0;
		while (1){
			if (lookfor[i] == 0){ 
				return (char *) searchee;
			}   
			if (lookfor[i] != searchee[i])
			{   
				break;
			}   
			i++;
		}   
		searchee++;
	}   
	return (char*) NULL;
}

