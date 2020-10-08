// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 */

#include <types.h>

uintptr_t strlen(const char *s)
{
	uintptr_t n = 0;

	while (*s++)
		n++;

	return n;
}
