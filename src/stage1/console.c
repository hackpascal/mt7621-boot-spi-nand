// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Simple console implementation
 */

#include <types.h>
#include <serial.h>

static const char hexchars[] = "0123456789abcdef";

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

void print_dec(uint32_t n)
{
	uint8_t chs[10];
	uint32_t t = 0;

	if (!n) {
		putc('0');
		return;
	}

	while (n) {
		chs[t++] = n % 10;
		n /= 10;
	}

	while (t) {
		putc('0' + chs[t - 1]);
		t--;
	}
}

void print_hex(uint32_t n, uint32_t padding)
{
	if (n)
		print_hex(n >> 4, padding);
	else
		return;

	putc(hexchars[n & 0xf]);
}

void print_hex2(uint8_t n)
{
	putc(hexchars[(n >> 4) & 0xf]);
	putc(hexchars[n & 0xf]);
}
