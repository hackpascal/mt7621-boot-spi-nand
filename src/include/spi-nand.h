/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Simple SPI-NAND driver
 */

#ifndef _SPI_NAND_H_
#define _SPI_NAND_H_

void spi_nand_init(void);
void spi_nand_addr_to_pa_ca(uint32_t addr, uint32_t *pa, uint32_t *ca);
void spi_nand_read_page_to_cache(uint32_t pa);
void spi_nand_read_page_cache(uint32_t ca, void *buf, uint32_t len);
void spi_nand_load(uint32_t addr, void *buf, uint32_t len, void *page_cache);

#endif /* _SPI_NAND_H_ */
