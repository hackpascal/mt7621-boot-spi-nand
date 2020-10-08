// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Simple memory pool
 */

#include <types.h>

static uintptr_t malloc_base;

void malloc_init(uintptr_t base)
{
	malloc_base = base;
}

void *malloc(size_t size)
{
	if (size < sizeof(uintptr_t))
		size = sizeof(uintptr_t);

	malloc_base -= size;
	malloc_base &= ~(16 - 1);

	return (void *)malloc_base;
}
