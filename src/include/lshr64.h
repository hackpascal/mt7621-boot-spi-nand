/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Implementation for 64-bit logical right shift
 */

#ifndef _LSHR64_H_
#define _LSHR64_H_

#include <types.h>

uint64_t __lshr64(uint64_t val, uint32_t sh);

#endif /* _LSHR64_H_ */
