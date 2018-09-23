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

/*
 * Copyright (c) 2014, STMicroelectronics International N.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This file provides what C99 standard requires in
 * 7.18 interger types <stdint.h>
 * with the exception of:
 * o Minimum-width integer types
 * o Fastest minimum-width integer types
 * o Geatest-width integer types
 */

#ifndef STDINT_H
#define STDINT_H
#define _STDINT_H

/*
 * If compiler supplies neither __ILP32__ or __LP64__, try to figure it out
 * here.
 */
#if !defined(__ILP32__) && !defined(__LP64__)
#if defined(__SIZEOF_INT__) && defined(__SIZEOF_POINTER__) && \
	defined(__SIZEOF_LONG__)
#if __SIZEOF_INT__ == 4 && __SIZEOF_POINTER__ == 4 && __SIZEOF_LONG__ == 4
#define __ILP32__ 1
#endif
#if __SIZEOF_INT__ == 4 && __SIZEOF_POINTER__ == 8 && __SIZEOF_LONG__ == 8
#define __LP64__ 1
#endif
#endif
#endif /* !defined(__ILP32__) && !defined(__LP64__) */

#if !defined(__ILP32__) && !defined(__LP64__)
#error Neither __ILP32__ nor __LP64__ is defined
#endif

/* 7.18.1.1 Exact-width interger types */
#ifndef __int8_t_defined
# define __int8_t_defined
typedef signed char             int8_t;
typedef short int               int16_t;
typedef int                     int32_t;
#ifdef __ILP32__
__extension__
typedef long long int           int64_t;
#endif /*__ILP32__*/
#ifdef __LP64__
typedef long int		int64_t;
#endif /*__LP64__*/
#endif

/* Unsigned.  */
typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
#ifndef __uint32_t_defined
typedef unsigned int            uint32_t;
# define __uint32_t_defined
#endif
#ifdef __ILP32__
__extension__
typedef unsigned long long int  uint64_t;
#endif /*__ILP32__*/
#ifdef __LP64__
typedef unsigned long int	uint64_t;
#endif /*__LP64__*/

/* 7.18.1.4 Integer types capable of holding object pointers */
typedef long intptr_t;
typedef unsigned long uintptr_t;

typedef int64_t intmax_t;
typedef uint64_t uintmax_t;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#ifndef NULL
#define NULL ((void*)0)
#endif


/*
 * 7.18.2 Limits of specified-width integer types
 */

/* 7.18.2.1 Limits of exact-width interger types */

#define INT8_MIN    (-0x7f-1)
#define INT16_MIN   (-0x7fff-1)
#define INT32_MIN   (-0x7fffffff-1)
#define INT64_MIN   (-0x7fffffffffffffffL-1)

#define INT8_MAX    0x7f
#define INT16_MAX   0x7fff
#define INT32_MAX   0x7fffffff
#define INT64_MAX   0x7fffffffffffffffL

#define UINT8_MAX    0xff
#define UINT16_MAX   0xffff
#define UINT32_MAX   0xffffffffU
#define UINT64_MAX   0xffffffffffffffffUL

/* 7.18.2.4 Limits of integer types capable of holding object pointers */

#define INTPTR_MIN  LONG_MIN
#define INTPTR_MAX  LONG_MAX
#define UINTPTR_MAX ULONG_MAX

#define INTMAX_MAX  INT64_MAX
#define INTMAX_MIN  INT64_MIN
#define UINTMAX_MAX UINT64_MAX

#endif /* STDINT_H */