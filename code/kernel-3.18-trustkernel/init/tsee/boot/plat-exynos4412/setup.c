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

#include <platform_config.h>

#include <stdint.h>
#include <string.h>
#include <trace.h>
#include <exynos4412_uart.h>

void console_init(void)
{
	uart_init((void*)CONSOLE_UART_BASE);
	uart_puts("console init\r\n");
}

void console_putc(int ch)
{
	uartputc(ch);
	if (ch == '\n')
		uartputc('\r');
}

void console_flush(void)
{
	
}

