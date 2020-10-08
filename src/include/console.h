/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Generic console functions
 */

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <types.h>

void putc(char c);
void puts(const char *s);

#if STAGE == 2
#include <stdarg.h>

int vprintf(const char *fmt, va_list args);
int printf(const char *fmt, ...);
#endif

#endif /* _CONSOLE_H_ */
