/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 */

#ifndef _CRC32_H_
#define _CRC32_H_

#include <types.h>

uint32_t crc32(uint32_t crc, const void *buf, size_t len);
void make_crc32_table(void);

#endif /* _CRC32_H_ */
