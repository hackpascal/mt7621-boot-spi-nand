// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 */

#include <types.h>

int memcmp(const void *cs, const void *ct, uintptr_t count)
{
	const uint8_t *su1 = cs, *su2 = ct;
	int res = 0;

	while (count) {
		res = *su1 - *su2;
		if (res)
			return res;

		su1++;
		su2++;
		count--;
	}

	return 0;
}
