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

// driver for Exynos uart
#include <stdint.h>
#include <platform_config.h>
#include <exynos4412_uart.h>
#include <trace.h>

struct exynos_uart {
	volatile unsigned int    ulcon;//0x0
	volatile unsigned int    ucon; //0x4
	volatile unsigned int    ufcon; //0x8
	volatile unsigned int    umcon; //0xc
	volatile unsigned int    utrstat; //0x10
	volatile unsigned int    uerstat; //0x14
	volatile unsigned int    ufstat; //0x18
	volatile unsigned int    umstat; //0x1c
	volatile unsigned char   utxh; //0x20
	volatile unsigned char   res1[3]; 
	volatile unsigned char   urxh; //0x24
	volatile unsigned char   res2[3];
	volatile unsigned int    ubrdiv; //0x28
	volatile unsigned int	 ufracval; //0x2c
	volatile unsigned int 	 uint32_tp; //0x30
	volatile unsigned int	 uint32_ts; //0x34
	volatile unsigned int 	 uint32_tm; //0x38
};


uint32_t *uart_base = (uint32_t *) UART0_BASE;

#define UART_DR		0	// data register
#define UART_RSR	1	// receive status register/error clear register
#define UART_FR		6	// flag register
#define	UART_IBRD	9	// integer baud rate register
#define UART_FBRD	10	// Fractional baud rate register
#define UART_LCR	11	// line control register
#define UART_CR		12	// control register
#define UART_IMSC	14	// interrupt mask set/clear register
#define UART_MIS	16	// masked interrupt status register
#define	UART_ICR	17	// interrupt clear register
// bits in registers
#define UARTFR_TXFF	(1 << 5)	// tramit FIFO full
#define UARTFR_RXFE	(1 << 4)	// receive FIFO empty
#define	UARTCR_RXE	(1 << 9)	// enable receive
#define UARTCR_TXE	(1 << 8)	// enable transmit
#define	UARTCR_EN	(1 << 0)	// enable UART
#define UARTLCR_FEN	(1 << 4)	// enable FIFO
#define UART_RXI	(1 << 4)	// receive interrupt
#define UART_TXI	(1 << 5)	// transmit interrupt
#define UART_BITRATE 19200
// enable the receive (interrupt) for uart (after GIC has initialized)
// reference: xen/xen/drivers/char/exynos4210-uart.c
void uart_enable_rx (void)
{
	struct exynos_uart* uart ;
	uart = (struct exynos_uart*)uart_base;
	//BUG: enable INT
	uart->uint32_tm = 0xe; //only unmask receiver interrupt 
	uart->uint32_tp = 0xf; //clear all pending interrupts 
	uart->umcon = 1<<3; //enable moderm interrupt
	uart->ucon |= ((1<<8)|1);//set receiver interrupt 
}

//poll the UART for data
int uartgetc (void)
{
	struct exynos_uart* uart ;
	uart = (struct exynos_uart*)uart_base;
	//check if rx fifo is full or if there is something in it 
	if((uart->ufstat &(1<<8)) || (uart->ufstat & 0xff)){
		return (uart->urxh & 0xff);
	}
	return -1;
}
void isr_uart (size_t it)
{
	struct exynos_uart* uart ;

	(void) it;

	uart = (struct exynos_uart*)uart_base;
	if((uart->ufstat &(1<<8)) || (uart->ufstat & 0xff)){
		//TODO
		EMSG("isr_uart: TODO\n");
	}
	//clear any interrupt
	uart->uint32_tp = ((1<<3)|(1<<2)|(1<<1)|(1<<0)); //clear pending interrupt 
}

//addr should be UART0
void uart_init(void *addr){
	struct exynos_uart* uart ;
	uart_base = addr;
	uart = (struct exynos_uart*)uart_base;

	/* reset FIFO */
	uart->ufcon = 0x111;
	/*no interrupt, auto flow control*/
	uart->umcon = 0x0;
	/* 8N1 */
	uart->ulcon = 0x3;
	/* No interrupts, no DMA, pure polling */
	uart->ucon = 0x3c5;
	//the value is given by my testing, maybe not valid
	uart->ubrdiv = 0x35;
	uart->ufracval = 0x4;
	/*
	// reset FIFO 
	//uart->ufcon = 0x111;
	uart->ufcon = 0x0;
	// no interrupt, auto flow control
	uart->umcon = 0x0;
	// 8N1 
	uart->ulcon = 0x3;
	// No interrupts, no DMA, pure polling 
	uart->ucon = 0x3c5;
	//the value is given by my testing, maybe not valid
	uart->ubrdiv = 0x35;
	uart->ufracval = 0x4;
	//mask and clear the interrupt 
	uart->uint32_tm = 0xf;
	uart->uint32_tp = 0xf;
	//enable interrupt 
	uart->ucon = (1<<0)|(1<<2)|(1<<8);
*/
}

void uartputc(int c){ 
	struct exynos_uart* uart ;
	uart = (struct exynos_uart*)uart_base;
	while(!(uart->utrstat & 0x2));
	uart->utxh = c;
}

void uart_puts(const char* c){ 
	int index = 0;
	while (c[index] != '\0'){ 
		uartputc(c[index]);
		index++;
	}   
}



