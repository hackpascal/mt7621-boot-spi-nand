/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Generic ns16550 UART registers
 */

#ifndef _NS16550_H_
#define _NS16550_H_

#include <bitops.h>

#define BASE_CLOCK_DIVISOR	16

#define UART_TBR		0x00
#define UART_DLL		UART_TBR
#define UART_IER		0x04
#define UART_DLM		UART_IER
#define UART_FCR		0x08
#define UART_LCR		0x0c
#define UART_LSR		0x14

#define LCR_DLAB		BIT(7)	/* Divisor Latch Access Bit */
#define LCR_WLS1		BIT(1)	/* Word Length Select */
#define LCR_WLS0		BIT(0)	/* Word Length Select */

#define LSR_TEMT		BIT(6)

#endif /* _NS16550_H_ */
