/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * SPI-NAND driver with bad block skipping mechanism
 */

#ifndef _SPI_NAND_LOAD_H_
#define _SPI_NAND_LOAD_H_

#include <types.h>

int spi_nand_load_init(void);

int spi_nand_read_page_to_cache(uint32_t pa);
int spi_nand_load_skip_bad(uint64_t addr, void *buff, uint32_t size);

#endif /* _SPI_NAND_LOAD_H_ */
