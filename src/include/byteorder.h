/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Generic byte swap functions
 */

#ifndef _BYTEORDER_H_
#define _BYTEORDER_H_

#include <types.h>

static inline uint32_t bswap32(uint32_t val)
{
	return ((val & 0x000000ffUL) << 24) |
	       ((val & 0x0000ff00UL) <<  8) |
	       ((val & 0x00ff0000UL) >>  8) |
	       ((val & 0xff000000UL) >> 24);
}

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define cpu_to_be32(x)	bswap32(x)
#define be32_to_cpu(x)	bswap32(x)
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define cpu_to_be32(x)	(x)
#define be32_to_cpu(x)	(x)
#else
#error Byte-order is not defined by the compiler!
#endif

#endif /* _BYTEORDER_H_ */
