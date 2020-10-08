/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Simple memory pool
 */

#ifndef _MALLOC_H_
#define _MALLOC_H_

#include <types.h>

void malloc_init(uintptr_t base);
void *malloc(size_t size);

#endif /* _MALLOC_H_ */
