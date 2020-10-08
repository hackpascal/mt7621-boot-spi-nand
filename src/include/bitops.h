/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Generic bit operations
 */

#ifndef _BITOPS_H_
#define _BITOPS_H_

#define BITS_PER_LONG _MIPS_SZLONG

#define BIT(x)		(1 << (x))

#define GENMASK(h, l) \
	(((~0UL) << (l)) & (~0UL >> (BITS_PER_LONG - 1 - (h))))

#ifndef __ASSEMBLY__
static inline unsigned long ffs(unsigned long word)
{
	int num = 1;

	if (!word)
		return 0;

#if BITS_PER_LONG == 64
	if ((word & 0xffffffff) == 0) {
		num += 32;
		word >>= 32;
	}
#endif

	if ((word & 0xffff) == 0) {
		num += 16;
		word >>= 16;
	}

	if ((word & 0xff) == 0) {
		num += 8;
		word >>= 8;
	}

	if ((word & 0xf) == 0) {
		num += 4;
		word >>= 4;
	}

	if ((word & 0x3) == 0) {
		num += 2;
		word >>= 2;
	}

	if ((word & 0x1) == 0)
		num += 1;

	return num;
}

static inline unsigned long ffs64(unsigned long long word)
{
#if BITS_PER_LONG == 64
	return ffs(word);
#else
	if (!word)
		return 0;

	if ((word & 0xffffffff) == 0)
		return ffs(word >> 32) + 32;

	return ffs(word & 0xffffffff);
#endif
}
#endif /* !__ASSEMBLY__ */

#endif /* _BITOPS_H_ */
