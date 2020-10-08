// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 */

#include <types.h>

void *memcpy(void *dst, const void *src, uintptr_t count)
{
	uintptr_t *dl = (uintptr_t *)dst, *sl = (uintptr_t *)src;
	uint8_t *d8, *s8;

	if (src == dst)
		return dst;

	if ((((uintptr_t) dst | (uintptr_t) src) & (sizeof(*dl) - 1)) == 0) {
		while (count >= sizeof(*dl)) {
			*dl++ = *sl++;
			count -= sizeof(*dl);
		}
	}

	d8 = (uint8_t *)dl;
	s8 = (uint8_t *)sl;

	while (count--)
		*d8++ = *s8++;

	return dst;
}
