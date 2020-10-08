// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Simple printf implementation
 */

#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <console.h>

static const char hex2chars[] = "0123456789abcdef";

static bool isdigit(char c)
{
	return c >= '0' && c <= '9';
}

static uint32_t uintptr_to_dec(uintptr_t value, char *buff)
{
	char tmpbuf[3 * sizeof(value)];
	uint32_t numdigits = 0, p;

	do {
		tmpbuf[numdigits++] = value % 10 + '0';
		value /= 10;
	} while (value);

	p = 0;
	while (p < numdigits) {
		*buff++ = tmpbuf[numdigits - p - 1];
		p++;
	}

	*buff = 0;

	return numdigits;
}

static uint32_t uintptr_to_hex(uintptr_t value, char *buff)
{
	char tmpbuf[2 * sizeof(value) + 1];
	uint32_t numdigits = 0, p;

	do {
		tmpbuf[numdigits++] = hex2chars[value & 0xf];
		value >>= 4;
	} while (value);

	p = 0;
	while (p < numdigits) {
		*buff++ = tmpbuf[numdigits - p - 1];
		p++;
	}

	*buff = 0;

	return numdigits;
}

static uintptr_t dec_to_uintptr(const char *str, char **end)
{
	uintptr_t value = 0, numdigits = 0;
	bool neg = false;

	if (end)
		*end = (char *)str;

	if (*str == '-') {
		neg = true;
		str++;
	}

	while (isdigit(*str)) {
		value *= 10;
		value += *str - '0';
		str++;
		numdigits++;
	}

	if (!numdigits)
		return 0;

	if (*end)
		*end = (char *)str;

	if (neg)
		value = -(intptr_t)value;

	return value;
}

static uint32_t print_fmt_one(const char *str, uintptr_t size, uintptr_t width,
			      bool zeropfx)
{
	uintptr_t bodylen;
	uint32_t numdigits = 0;
	char padding;

	if (zeropfx)
		padding = '0';
	else
		padding = ' ';

	bodylen = size;

	while (width > bodylen) {
		putc(padding);
		numdigits++;
		width--;
	}

	while (size) {
		putc(*str++);
		numdigits++;
		size--;
	}

	return numdigits;
}

int vprintf(const char *fmt, va_list args)
{
	uint32_t numdigits = 0, buflen;
	uintptr_t width, slen;
	bool zeropfx;
	uintptr_t num;
	char buff[32], *s;
	const char *fmt_start = fmt;

	while (*fmt) {
		width = 0;

		if (*fmt != '%') {
			if (*fmt == '\n') {
				if (fmt > fmt_start) {
					if (fmt[-1] != '\r')
						putc('\r');
				} else {
					if (fmt[1] == 0)
						putc('\r');
				}
			}

			putc(*fmt++);
			numdigits++;
			continue;
		}

		zeropfx = false;
		fmt++;

		if (*fmt == '0') {
			fmt++;
			if (isdigit(*fmt)) {
				width = dec_to_uintptr(fmt, (char **)&fmt);
				zeropfx = true;
			}
		}

		switch (*fmt) {
		case 'u':
			num = va_arg(args, unsigned int);
			buflen = uintptr_to_dec(num, buff);
			numdigits += print_fmt_one(buff, buflen, width, zeropfx);
			break;
		case 'x':
			num = va_arg(args, unsigned int);
			buflen = uintptr_to_hex(num, buff);
			numdigits += print_fmt_one(buff, buflen, width, zeropfx);
			break;
		case 's':
			s = va_arg(args, char *);
			slen = strlen(s);
			numdigits += print_fmt_one(s, slen, width, zeropfx);
			break;
		case '%':
			putc(*fmt++);
			numdigits++;
			break;
		default:
			putc('%');
			numdigits++;

			if (*fmt) {
				putc(*fmt);
				numdigits++;
				break;
			}

			return numdigits;
		}

		fmt++;
	}

	return numdigits;
}

int printf(const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = vprintf(fmt, args);
	va_end(args);

	return len;
}
