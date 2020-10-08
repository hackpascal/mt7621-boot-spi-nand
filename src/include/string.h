/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Standard C library string functions
 */

#ifndef _STRING_H_
#define _STRING_H_

#include <types.h>

void *memset(void *dst, int val, uintptr_t count);
void *memcpy(void *dst, const void *src, uintptr_t count);
int memcmp(const void *cs, const void *ct, uintptr_t count);
uintptr_t strlen(const char *s);

#endif /* _STRING_H_ */
