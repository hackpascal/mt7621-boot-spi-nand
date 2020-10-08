// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Simple UART driver for MediaTek MT7621
 */

#include <types.h>
#include <mmio.h>
#include <ns16550.h>
#include <mt7621_regs.h>

#define UART_BASE		UART1_BASE
#define BASE_CLOCK		50000000

#define DIV_ROUND_CLOSEST(x, divisor) \
	({ \
	typeof(divisor) __divisor = divisor; \
	(((x) + ((__divisor) / 2)) / (__divisor)); \
	})

extern uint32_t _baudrate;

void serial_init(void)
{
	uint32_t clock_divisor;

	clock_divisor = DIV_ROUND_CLOSEST(BASE_CLOCK, (BASE_CLOCK_DIVISOR * _baudrate));

	reg_clr32(SYSCTL_GPIOMODE, UART1_MODE_M);

	reg_write32(UART_BASE + UART_IER, 0);
	reg_write32(UART_BASE + UART_FCR, 0);

	/* set baud rate */
	reg_write32(UART_BASE + UART_LCR, LCR_WLS0 | LCR_WLS1 | LCR_DLAB);
	reg_write32(UART_BASE + UART_DLL, clock_divisor & 0xff);
	reg_write32(UART_BASE + UART_DLM, clock_divisor >> 8);
	reg_write32(UART_BASE + UART_LCR, LCR_WLS0 | LCR_WLS1);
}

void serial_putc(char c)
{
	/* wait for room in the tx FIFO on UART */
	while ((reg_read32(UART_BASE + UART_LSR) & LSR_TEMT) == 0);

	reg_write32(UART_BASE + UART_TBR, c);
}
