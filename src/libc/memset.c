// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 */

#include <types.h>

void *memset(void *dst, int val, uintptr_t count)
{
	char *ptr = dst;

	while (count--)
		*ptr++ = val;

	return dst;
}
