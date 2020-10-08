// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Implementation for 64-bit logical right shift
 */

#include <types.h>

uint64_t __lshr64(uint64_t val, uint32_t sh)
{
	uint32_t hi, lo;

	hi = val >> 32;
	lo = val & 0xffffffff;

	if (!hi)
		return lo >> sh;

	if (sh >= 32)
		return hi >> (sh - 32);

	if (!sh)
		return val;

	return ((uint64_t)(hi >> sh) << 32) | (hi << (32 - sh)) | (lo >> sh);
}
