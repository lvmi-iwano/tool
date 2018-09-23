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

#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H

//tiny4412
//#define UART0_BASE		0x13800000
// rp4412
#define UART0_BASE		0x13820000

#define IT_UART0		84

#define CONSOLE_UART_BASE	UART0_BASE
#define IT_CONSOLE_UART		IT_UART0
#define CONSOLE_BAUDRATE	115200
#define CONSOLE_UART_CLK_IN_HZ	19200000

#define DRAM0_BASE		0x01000000
#define DRAM0_SIZE		0x08000000
#define DRAM1_BASE		0x40000000
#define DRAM1_SIZE		0x40000000

#define DEVRAM_BASE		0x10000000
#define DEVRAM_SIZE     0x08000000

#define CFG_TSEE_LOAD_ADDR	0x50000000

#define CFG_TSEE_RAM_SIZE	(1024 * 1024 * 1)


#endif /*PLATFORM_CONFIG_H*/
