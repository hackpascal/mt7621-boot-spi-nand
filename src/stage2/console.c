// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 */

#include <types.h>
#include <serial.h>

void putc(char c)
{
	serial_putc(c);
}

void puts(const char *s)
{
	while (*s) {
		if (*s == '\n')
			putc('\r');
		putc(*s++);
	}
}
