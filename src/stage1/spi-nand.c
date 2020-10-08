// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Simple SPI-NAND driver for 1st-stage
 */

#include <mmio.h>
#include <bitops.h>
#include <timer.h>
#include <serial.h>
#include <string.h>
#include <compiler.h>
#include <spi.h>
#include <spi-nand.h>

#define SPI_NAND_CMD_READ_CACHE		0x03
#define SPI_NAND_CMD_GET_FEATURE	0x0f
#define SPI_NAND_CMD_READ_PAGE_TO_CACHE	0x13
#define SPI_NAND_CMD_SET_FEATURE	0x1f
#define SPI_NAND_CMD_RESET		0xff

#define SPI_NAND_FEATURE_CONFIG_ADDR	0xb0
#define SPI_NAND_CR_BIT_ECCEN		0x10

#define SPI_NAND_FEATURE_STATUS		0xc0
#define SPI_NAND_SR_BIT_OIP		0x01

#define SPI_NAND_TIMING_RESET		1000

#define PLANEBIT			((PAGESIZE) << 1)

static void spi_nand_reset(void)
{
	uint8_t op = SPI_NAND_CMD_RESET;

	spi_en();
	spi_write(&op, 1);
	spi_dis();

	udelay(SPI_NAND_TIMING_RESET);
}

static uint8_t spi_nand_get_feature(uint8_t addr)
{
	uint8_t op[2] = { SPI_NAND_CMD_GET_FEATURE, addr };
	uint8_t val;

	spi_en();
	spi_write(op, sizeof(op));
	spi_read(&val, 1);
	spi_dis();

	return val;
}

static void spi_nand_set_feature(uint8_t addr, uint8_t val)
{
	uint8_t op[3] = { SPI_NAND_CMD_SET_FEATURE, addr, val };

	spi_en();
	spi_write(op, sizeof(op));
	spi_dis();
}

static void spi_nand_enable_ecc(void)
{
	uint8_t val;

	val = spi_nand_get_feature(SPI_NAND_FEATURE_CONFIG_ADDR);
	val |= SPI_NAND_CR_BIT_ECCEN;
	spi_nand_set_feature(SPI_NAND_FEATURE_CONFIG_ADDR, val);
}

static void spi_nand_poll(void)
{
	uint8_t op[2] = { SPI_NAND_CMD_GET_FEATURE, SPI_NAND_FEATURE_STATUS };
	uint8_t val;

	while (1) {
		spi_en();
		spi_write(op, sizeof(op));
		spi_read(&val, sizeof(val));
		spi_dis();

		if ((val & SPI_NAND_SR_BIT_OIP) == 0)
			break;
	}
}

void spi_nand_init(void)
{
	spi_nand_reset();
	spi_nand_enable_ecc();
}

void spi_nand_addr_to_pa_ca(uint32_t addr, uint32_t *pa, uint32_t *ca)
{
	*pa = addr / PAGESIZE;
	*ca = addr & (PAGESIZE - 1);

	if ((addr / BLOCKSIZE) & 1)
		*ca |= PLANEBIT;
}

void spi_nand_read_page_to_cache(uint32_t pa)
{
	uint8_t op[4];

	op[0] = SPI_NAND_CMD_READ_PAGE_TO_CACHE;
	op[1] = (pa >> 16) & 0xff;
	op[2] = (pa >> 8) & 0xff;
	op[3] = pa & 0xff;

	spi_en();
	spi_write(op, sizeof(op));
	spi_dis();

	spi_nand_poll();
}

void spi_nand_read_page_cache(uint32_t ca, void *buf, uint32_t len)
{
	uint8_t op[4];

	op[0] = SPI_NAND_CMD_READ_CACHE;
	op[1] = 0;
	op[2] = (ca >> 8) & 0xff;
	op[3] = ca & 0xff;

	spi_en();
	spi_write(op, sizeof(op));
	spi_read(buf, len);
	spi_dis();
}

static void spi_nand_read_page(uint32_t addr, void *buf, uint32_t len)
{
	uint32_t pa, ca;

	spi_nand_addr_to_pa_ca(addr, &pa, &ca);

	spi_nand_read_page_to_cache(pa);
	spi_nand_read_page_cache(ca, buf, len);
}

void spi_nand_load(uint32_t addr, void *buf, uint32_t len, void *page_cache)
{
	uint32_t chksz, leading;
	uint32_t off = addr, curr_addr;
	uint8_t *ptr = buf;

	while (len) {
		leading = off & (PAGESIZE - 1);
		chksz = PAGESIZE - leading;
		if (chksz > len)
			chksz = len;

		curr_addr = off & (~(PAGESIZE - 1));
		spi_nand_read_page(curr_addr, page_cache, PAGESIZE);

		memcpy(ptr, page_cache + leading, chksz);

		off += chksz;
		ptr += chksz;
		len -= chksz;
	}
}
