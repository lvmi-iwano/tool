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

#ifndef COMPILER_H
#define COMPILER_H

/*
 * Macros that should be used instead of using __attributue__ directly to
 * ease portability and make the code easier to read.
 */

#define __deprecated	__attribute__((deprecated))
#define __packed	__attribute__((packed))
#define __weak		__attribute__((weak))
#define __noreturn	__attribute__((noreturn))
#define __pure		__attribute__((pure))
#define __aligned(x)	__attribute__((aligned(x)))
#define __printf(a, b)	__attribute__((format(printf, a, b)))
#define __noinline	__attribute__((noinline))
#define __attr_const	__attribute__((__const__))
#define __unused	__attribute__((unused))
#define __used		__attribute__((__used__))
#define __must_check	__attribute__((warn_unused_result))
#define __cold		__attribute__((__cold__))
#define __section(x)	__attribute__((section(x)))
#define __data		__section(".data")
#define __bss		__section(".bss")
#define __rodata	__section(".rodata")

#endif /*COMPILER_H*/
