/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Generic cache manipluation for MIPS platform
 */

#ifndef _CACHE_H_
#define _CACHE_H_

#ifndef __ASSEMBLY__
#include <compiler.h>
#include <types.h>
#endif

#define INDEX_STORE_TAG_I		0x08
#define INDEX_STORE_TAG_D		0x09
#define INDEX_STORE_TAG_SD		0x0b
#define INDEX_STORE_DATA_SD		0x0f
#define HIT_INVALIDATE_I		0x10
#define HIT_INVALIDATE_D		0x11
#define HIT_INVALIDATE_SD		0x13
#define HIT_WRITEBACK_INV_D		0x15
#define HIT_WRITEBACK_INV_SD		0x17

#ifndef __ASSEMBLY__
extern void __nomips16 invalidate_l1_cache_range(uintptr_t base, uint32_t size);
extern void __nomips16 flush_l1_cache_range(uintptr_t base, uint32_t size);
extern void __nomips16 reset_l1_cache(void);
#endif

#endif /* _CACHE_H_ */
