// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Wrapper for LZMA decompression
 */

#include <errno.h>
#include <console.h>
#include <malloc.h>

#include "LzmaDec.h"

#define LZMA_DATA_OFFSET	(LZMA_PROPS_SIZE + sizeof(uint64_t))

static void *lzma_malloc(void *p, size_t size)
{
	return malloc(size);
}

static void lzma_free(void *p, void *address)
{
}

int lzma_uncompress(const void *comp_data, uint32_t comp_size,
		    void *decomp_data, uint32_t *decomp_size)
{
	int i, ret;
	const uint8_t *p;
	ELzmaStatus state;
	ISzAlloc lzma_alloc;
	SizeT dst_len, src_len;
	uint32_t real_decomp_size = 0;

	p = comp_data + LZMA_PROPS_SIZE;

	for (i = 0; i < 4; i++)
		real_decomp_size |= (uint32_t)p[i + 4] << (i * 8);

	if (real_decomp_size) {
		puts("LZMA uncompressed data is too large\n");
		return -ENOTSUPP;
	}

	real_decomp_size = 0;

	for (i = 0; i < 4; i++)
		real_decomp_size |= (uint32_t)p[i] << (i * 8);

	if (!*decomp_size)
		dst_len = real_decomp_size;
	else
		dst_len = real_decomp_size < *decomp_size ? real_decomp_size :
							    *decomp_size;

	src_len = comp_size - LZMA_PROPS_SIZE;

	lzma_alloc.Alloc = lzma_malloc;
	lzma_alloc.Free = lzma_free;

	memset(&state, 0, sizeof(state));

	ret = LzmaDecode(decomp_data, &dst_len, comp_data + LZMA_DATA_OFFSET,
			 &src_len, comp_data, LZMA_PROPS_SIZE, LZMA_FINISH_END,
			 &state, &lzma_alloc);
	if (ret != SZ_OK)
		return ret;

	*decomp_size = dst_len;

	return 0;
}
