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


#ifndef EXYNOS4412_UART_H
#define EXYNOS4412_UART_H

void uart_enable_rx (void);
int uartgetc (void);
void uart_init(void *addr);
void uart_puts(const char* c);
void uartputc(int c);

#endif /* EXYNOS4412_UART_H */

