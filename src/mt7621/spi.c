// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * SPI controller driver for MT7621
 */

#include <mmio.h>
#include <bitops.h>
#include <mt7621_regs.h>
#include <spi.h>

#define MT7621_RX_FIFO_LEN		32
#define MT7621_TX_FIFO_LEN		36

void spi_init(void)
{
	reg_write32(SPI_MASTER_REG, (RS_SLAVE_SEL_MASK << RS_SLAVE_SEL_SHIFT) |
		    (12 << RS_CLK_SEL_SHIFT) | MORE_BUF_MODE);
	reg_write32(SPI_CS_POLAR_REG, 0);
}

void spi_set_clock(uint32_t hclk, uint32_t clk)
{
	uint32_t div, val;

	div = (hclk + clk - 1) / clk;

	if (div > 4097)
		div = 4097;

	if (div < 2)
		div = 2;

	div -= 2;

	val = reg_read32(SPI_MASTER_REG);
	val &= ~(RS_CLK_SEL_MASK << RS_CLK_SEL_SHIFT);
	val |= div << RS_CLK_SEL_SHIFT;
	reg_write32(SPI_MASTER_REG, val);
}

void spi_en(void)
{
	reg_write32(SPI_CS_POLAR_REG, 1);
}

void spi_dis(void)
{
	reg_write32(SPI_CS_POLAR_REG, 0);
}

static void spi_busy_wait(void)
{
	while (reg_read32(SPI_TRANS_REG) & SPI_MASTER_BUSY);
}

void spi_read(uint8_t *buf, uint32_t len)
{
	uint32_t rx_len, val = 0;
	int i;

	while (len) {
		rx_len = len < MT7621_RX_FIFO_LEN ? len : MT7621_RX_FIFO_LEN;

		reg_write32(SPI_MOREBUF_REG, (rx_len * 8) << MISO_BIT_CNT_SHIFT);
		reg_write32(SPI_TRANS_REG, SPI_MASTER_START);

		spi_busy_wait();

		for (i = 0; i < rx_len; i++) {
			if ((i % 4) == 0)
				val = reg_read32(SPI_DIDO_REG(i / 4));
			*buf++ = val & 0xff;
			val >>= 8;
		}

		len -= rx_len;
	}
}

void spi_write(const uint8_t *buf, uint32_t len)
{
	uint32_t tx_len, opcode_len, dido_len, val;
	int i;

	while (len) {
		tx_len = len < MT7621_TX_FIFO_LEN ? len: MT7621_TX_FIFO_LEN;

		opcode_len = tx_len < 4 ? tx_len : 4;
		dido_len = tx_len - opcode_len;

		val = 0;
		for (i = 0; i < opcode_len; i++) {
			val <<= 8;
			val |= *buf++;
		}

		reg_write32(SPI_OP_ADDR_REG, val);

		val = 0;
		for (i = 0; i < dido_len; i++) {
			val |= (*buf++) << ((i % 4) * 8);

			if ((i % 4 == 3) || (i == dido_len - 1)) {
				reg_write32(SPI_DIDO_REG(i / 4), val);
				val = 0;
			}
		}

		reg_write32(SPI_MOREBUF_REG,
			   ((opcode_len * 8) << CMD_BIT_CNT_SHIFT) |
			   ((dido_len * 8) << MOSI_BIT_CNT_SHIFT));
		reg_write32(SPI_TRANS_REG, SPI_MASTER_START);

		spi_busy_wait();

		len -= tx_len;
	}
}
