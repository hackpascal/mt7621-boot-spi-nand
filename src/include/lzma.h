/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Wrapper for LZMA decompression
 */

#ifndef _LZMA_H_
#define _LZMA_H_

#include <types.h>

int lzma_uncompress(const void *comp_data, uint32_t comp_size,
		    void *decomp_data, uint32_t *decomp_size);

#endif /* _LZMA_H_ */
