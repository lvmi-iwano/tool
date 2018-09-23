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

#ifndef __THEE_H__
#define __THEE_H__

#define THEE_RUNTIME_BASE 0x900000
#define THEE_RUNTIME_SIZE 0x400000

#ifndef __ASSEMBLY__

int thee_init(void);

#endif //__ASSEMBLY__

#endif //__THEE_H__
