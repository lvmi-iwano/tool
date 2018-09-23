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

#ifndef UTIL_H
#define UTIL_H

#ifndef MAX
#define MAX(a, b) \
	(__extension__({ __typeof__(a) _a = (a); \
	   __typeof__(b) _b = (b); \
	 _a > _b ? _a : _b; }))

#define MIN(a, b) \
	(__extension__({ __typeof__(a) _a = (a); \
	   __typeof__(b) _b = (b); \
	 _a < _b ? _a : _b; }))
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/* Round up the even multiple of size, size has to be a multiple of 2 */
#define ROUNDUP(v, size) (((v) + ((size) - 1)) & ~((size) - 1))

/* Round down the even multiple of size, size has to be a multiple of 2 */
#define ROUNDDOWN(v, size) ((v) & ~((size) - 1))

/* x has to be of an unsigned type */
#define IS_POWER_OF_TWO(x) (((x) != 0) && (((x) & (~(x) + 1)) == (x)))

#define TO_STR(x) _TO_STR(x)
#define _TO_STR(x) #x

#endif /*UTIL_H*/
