/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * SPI controller driver for MT7621
 */

#ifndef _SPI_H_
#define _SPI_H_

#include <types.h>

void spi_init(void);
void spi_set_clock(uint32_t hclk, uint32_t clk);
void spi_en(void);
void spi_dis(void);
void spi_read(uint8_t *buf, uint32_t len);
void spi_write(const uint8_t *buf, uint32_t len);

#endif /* _SPI_H_ */
