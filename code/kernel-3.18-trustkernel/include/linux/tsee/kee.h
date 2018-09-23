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

#ifndef __KEE_H__
#define __KEE_H__

#define KEE_RUNTIME_BASE 0x900000
#define KEE_RUNTIME_SIZE 0x400000

#ifndef __ASSEMBLY__

int kee_init(void);

#endif //__ASSEMBLY__

#endif //__KEE_H__
