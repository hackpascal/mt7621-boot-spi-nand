// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * SPI-NAND driver with bad block skipping mechanism
 */

#include <types.h>
#include <errno.h>
#include <bitops.h>
#include <stddef.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include <genfuncs.h>
#include <timer.h>
#include <console.h>
#include <lshr64.h>
#include <spi.h>

#define SPI_NAND_CMD_READ_CACHE			0x03
#define SPI_NAND_CMD_GET_FEATURE		0x0f
#define SPI_NAND_CMD_READ_PAGE_TO_CACHE		0x13
#define SPI_NAND_CMD_SET_FEATURE		0x1f
#define SPI_NAND_CMD_READ_ID			0x9f
#define SPI_NAND_CMD_RESET			0xff

#define SPI_NAND_FEATURE_CONFIG_ADDR		0xb0
#define SPI_NAND_CR_BIT_ECCEN			BIT(4)

#define SPI_NAND_FEATURE_STATUS			0xc0
#define SPI_NAND_SR_BIT_OIP			BIT(0)

#define SPI_NAND_FEATURE_MICRON_DIE_ADDR	0xd0
#define SPI_NAND_MICRON_DIE_SEL_1		BIT(6)

#define TIMEOUT					1000000
#define SPI_NAND_TIMING_RESET			1000


#define SPI_NAND_ID_MAX_LEN			4

struct spi_nand_id {
	uint8_t len;
	uint8_t id[SPI_NAND_ID_MAX_LEN];
};

#define SPI_NAND_ID(...) \
	{ .id = { __VA_ARGS__ }, .len = sizeof((uint8_t[]){ __VA_ARGS__ }) }

struct spi_nand_memorg {
	uint16_t pagesize;
	uint16_t sparesize;
	uint16_t pages_per_block;
	uint16_t blocks_per_die;
	uint16_t planes_per_die;
	uint16_t ndies;
};

#define SPI_NAND_MEMORG(_ps, _ss, _ppb, _bpd, _ppd, _nd) \
	{ .pagesize = (_ps), .sparesize = (_ss), .pages_per_block = (_ppb), \
	  .blocks_per_die = (_bpd), .planes_per_die = (_ppd), .ndies = (_nd)}

struct spi_nand_ecccfg {
	uint16_t msb;
	uint16_t lsb;
	const int8_t *bitflips;
};

#define SPI_NAND_ECCCFG(_msb, _lsb, _bitflips) \
	{ .msb = (_msb), .lsb = (_lsb), .bitflips = (_bitflips) }

typedef int (*snand_select_die_t)(uint32_t dieidx);

struct spi_nand_info {
	const char *model;
	struct spi_nand_id id;
	struct spi_nand_memorg memorg;
	struct spi_nand_ecccfg ecccfg;
	snand_select_die_t select_die;
};

#define SPI_NAND_INFO(_model, _id, _memorg, _ecccfg, ...) \
	{ .model = (_model), .id = _id, .memorg = _memorg, .ecccfg = _ecccfg, \
	  __VA_ARGS__ }

#define SPI_NAND_MEMORG_1G_2K_64	SPI_NAND_MEMORG(2048, 64, 64, 1024, 1, 1)
#define SPI_NAND_MEMORG_1G_2K_128	SPI_NAND_MEMORG(2048, 128, 64, 1024, 1, 1)
#define SPI_NAND_MEMORG_2G_2K_128_2P	SPI_NAND_MEMORG(2048, 128, 64, 2048, 2, 1)
#define SPI_NAND_MEMORG_4G_4K_256	SPI_NAND_MEMORG(4096, 256, 64, 2048, 1, 1)
#define SPI_NAND_MEMORG_4G_2K_128_2P_2D	SPI_NAND_MEMORG(2048, 128, 64, 2048, 2, 2)
#define SPI_NAND_MEMORG_8G_4K_256_2D	SPI_NAND_MEMORG(4096, 256, 64, 2048, 1, 2)

static int micron_select_die(uint32_t dieidx);

static const int8_t ecc_bitflips_2[] = { 0, 4, -1, -1 };
static const int8_t ecc_bitflips_3[] = { 0, 3, -1, 6, -1, 8, -1, -1 };

static const struct spi_nand_info spi_nand_ids[] = {
	SPI_NAND_INFO("MT29F1G01AAADD", SPI_NAND_ID(0x2c, 0x12),
		      SPI_NAND_MEMORG_1G_2K_64,
		      SPI_NAND_ECCCFG(5, 4, ecc_bitflips_2)),
	SPI_NAND_INFO("MT29F1G01ABAFD", SPI_NAND_ID(0x2c, 0x14),
		      SPI_NAND_MEMORG_1G_2K_128,
		      SPI_NAND_ECCCFG(6, 4, ecc_bitflips_3)),
	SPI_NAND_INFO("MT29F2G01ABAGD", SPI_NAND_ID(0x2c, 0x24),
		      SPI_NAND_MEMORG_2G_2K_128_2P,
		      SPI_NAND_ECCCFG(6, 4, ecc_bitflips_3)),
	SPI_NAND_INFO("MT29F4G01ABAFD", SPI_NAND_ID(0x2c, 0x34),
		      SPI_NAND_MEMORG_4G_4K_256,
		      SPI_NAND_ECCCFG(6, 4, ecc_bitflips_3)),
	SPI_NAND_INFO("MT29F4G01ADAGD", SPI_NAND_ID(0x2c, 0x36),
		      SPI_NAND_MEMORG_4G_2K_128_2P_2D,
		      SPI_NAND_ECCCFG(6, 4, ecc_bitflips_3),
		      micron_select_die),
	SPI_NAND_INFO("MT29F8G01ADAFD", SPI_NAND_ID(0x2c, 0x46),
		      SPI_NAND_MEMORG_8G_4K_256_2D,
		      SPI_NAND_ECCCFG(6, 4, ecc_bitflips_3),
		      micron_select_die),
};

static const struct spi_nand_info *spi_nand_chip;
static uint32_t writesize, writesize_mask, writesize_shift;
static uint32_t erasesize, erasesize_mask, erasesize_shift;
static uint64_t diesize, diesize_mask, chipsize;
static uint32_t diesize_shift, ndies, oobsize;
static uint32_t eccmask, eccshift;
static const int8_t *ecc_bitflips;
static snand_select_die_t select_die;
static uint8_t *page_cache;
static uint32_t curr_dieidx;

static void spi_nand_reset(void)
{
	uint8_t op = SPI_NAND_CMD_RESET;

	spi_en();
	spi_write(&op, 1);
	spi_dis();

	udelay(SPI_NAND_TIMING_RESET);
}

static void spi_nand_read_id(uint8_t *id)
{
	uint8_t op[2] = { SPI_NAND_CMD_READ_ID, 0 };

	spi_en();
	spi_write(op, sizeof(op));
	spi_read(id, 4);
	spi_dis();
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

static int micron_select_die(uint32_t dieidx)
{
	uint8_t val;

	val = spi_nand_get_feature(SPI_NAND_FEATURE_MICRON_DIE_ADDR);
	if (!dieidx)
		val &= ~SPI_NAND_MICRON_DIE_SEL_1;
	else
		val |= SPI_NAND_MICRON_DIE_SEL_1;
	spi_nand_set_feature(SPI_NAND_FEATURE_MICRON_DIE_ADDR, val);

	return 0;
}

static int spi_nand_select_die(uint32_t dieidx)
{
	int ret;

	if (ndies == 1 || !select_die)
		return 0;

	if (dieidx >= ndies) {
		printf("Invalid die index: %u\n", dieidx);
		return -EINVAL;
	}

	if (curr_dieidx == dieidx)
		return 0;

	ret = select_die(dieidx);
	if (!ret)
		curr_dieidx = dieidx;

	return ret;
}

static int spi_nand_poll(void)
{
	uint8_t val;
	uint64_t start_time = get_timer(0);

	while (1) {
		val = spi_nand_get_feature(SPI_NAND_FEATURE_STATUS);

		if ((val & SPI_NAND_SR_BIT_OIP) == 0)
			return 0;

		if (get_timer(start_time) > TIMEOUT)
			return -ETIMEDOUT;
	}
}

static void spi_nand_init_chip(void)
{
	uint32_t i;

	for (i = 0; i < ndies; i++) {
		spi_nand_select_die(i);
		spi_nand_enable_ecc();
	}

	if (ndies > 1)
		spi_nand_select_die(0);
}

static void spi_nand_addr_to_pa_ca(uint64_t addr, uint32_t *pa, uint32_t *ca)
{
	*pa = __lshr64(addr, writesize_shift);
	*ca = addr & writesize_mask;

	if (__lshr64(addr, erasesize_shift) & 1)
		*ca |= (1 << (writesize_shift + 1));
}

int spi_nand_read_page_to_cache(uint32_t pa)
{
	uint8_t op[4];

	op[0] = SPI_NAND_CMD_READ_PAGE_TO_CACHE;
	op[1] = (pa >> 16) & 0xff;
	op[2] = (pa >> 8) & 0xff;
	op[3] = pa & 0xff;

	spi_en();
	spi_write(op, sizeof(op));
	spi_dis();

	return spi_nand_poll();
}

static void spi_nand_read_page_cache(uint32_t ca, void *buf, uint32_t len)
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

static int spi_nand_get_ecc_status(void)
{
	uint8_t val = spi_nand_get_feature(SPI_NAND_FEATURE_STATUS);
	uint32_t ecc_status = (val & eccmask) >> eccshift;

	if (ecc_bitflips[ecc_status] < 0)
		return -EBADMSG;

	return ecc_bitflips[ecc_status];
}

static int spi_nand_read_page(uint64_t addr, void *buf, uint32_t len)
{
	uint32_t pa, ca, dieidx = 0;
	uint64_t die_addr = addr;
	int ret;

	if (addr >= chipsize)
		return -EINVAL;

	if (ndies > 1) {
		die_addr = addr & diesize_mask;
		dieidx = __lshr64(addr, diesize_shift);
	}

	spi_nand_select_die(dieidx);

	spi_nand_addr_to_pa_ca(die_addr, &pa, &ca);

	if ((ca & writesize_mask) + len >= writesize + oobsize)
		return -EINVAL;

	ret = spi_nand_read_page_to_cache(pa);
	if (ret) {
		spi_nand_reset();
		spi_nand_init_chip();
		return ret;
	}

	ret = spi_nand_get_ecc_status();

	spi_nand_read_page_cache(ca, buf, len);

	if (ret < 0)
		return ret;

	return 0;
}

static void spi_nand_print_error(int ret, uint64_t addr)
{
	uint32_t page = __lshr64(addr, writesize_shift);

	if (ret == -EBADMSG)
		printf("Uncorrectable bitflips in page %u\n", page);
	else if (ret == -ETIMEDOUT)
		printf("Timed out while reading page %u\n", page);
	else
		printf("Failed to read page %u\n", page);
}

int spi_nand_load_init(void)
{
	uint8_t id[SPI_NAND_ID_MAX_LEN];
	const struct spi_nand_id *fid = NULL;
	uint32_t i;

	spi_nand_reset();
	spi_nand_read_id(id);

	for (i = 0; i < ARRAY_SIZE(spi_nand_ids); i++) {
		fid = &spi_nand_ids[i].id;

		if (!memcmp(fid->id, id, fid->len)) {
			spi_nand_chip = &spi_nand_ids[i];
			break;
		}
	}

	if (!spi_nand_chip) {
		printf("SPI-NAND: Unrecognized ID: %02x %02x %02x %02x\n",
		       id[0], id[1], id[2], id[3]);
		return -ENODEV;
	}

	oobsize = spi_nand_chip->memorg.sparesize;

	writesize = spi_nand_chip->memorg.pagesize;
	writesize_mask = writesize - 1;
	writesize_shift = ffs(writesize) - 1;

	erasesize = spi_nand_chip->memorg.pagesize *
		    spi_nand_chip->memorg.pages_per_block;
	erasesize_mask = erasesize - 1;
	erasesize_shift = ffs(erasesize) - 1;

	diesize = (uint64_t)erasesize * spi_nand_chip->memorg.blocks_per_die;
	diesize_mask = diesize - 1;
	diesize_shift = ffs64(diesize) - 1;

	ndies = spi_nand_chip->memorg.ndies;
	chipsize = diesize * ndies;

	eccmask = GENMASK(spi_nand_chip->ecccfg.msb, spi_nand_chip->ecccfg.lsb);
	eccshift = spi_nand_chip->ecccfg.lsb;
	ecc_bitflips = spi_nand_chip->ecccfg.bitflips;

	select_die = spi_nand_chip->select_die;

	page_cache = malloc(writesize + oobsize);

	spi_nand_init_chip();

	printf("SPI-NAND: %s (%uMB)\n", spi_nand_chip->model,
	       (uint32_t)(__lshr64(chipsize, 20)));

	return 0;
}

int spi_nand_load_skip_bad(uint64_t addr, void *buff, uint32_t size)
{
	uint32_t chksz, leading, blkofs;
	uint64_t off = addr, curr_addr;
	bool curr_page_read, check_bad = true;
	uint8_t *ptr = buff;
	int ret;

	while (size) {
		if (off >= chipsize) {
			printf("Reading exceeds flash size, %u bytes left\n",
			       size);
			return -EIO;
		}

		curr_page_read = false;
		leading = off & writesize_mask;
		chksz = writesize - leading;
		if (chksz > size)
			chksz = size;

		if ((off & erasesize_mask) == 0 || check_bad) {
			curr_addr = off & (~erasesize_mask);
			ret = spi_nand_read_page(curr_addr, page_cache,
						 writesize + 1);
			if (ret < 0) {
				spi_nand_print_error(ret, curr_addr);
				goto skip_block;
			}

			if (page_cache[writesize] != 0xff) {
				printf("Bad block at 0x%x\n", curr_addr);
				goto skip_block;
			}

			if (curr_addr == (off & (~writesize_mask)))
				curr_page_read = true;

			check_bad = false;
		}

		if (!curr_page_read) {
			curr_addr = off & (~writesize_mask);
			ret = spi_nand_read_page(curr_addr, page_cache, writesize);
			if (ret < 0) {
				spi_nand_print_error(ret, curr_addr);
				if (ret != -EBADMSG)
					return ret;
			}
		}

		memcpy(ptr, page_cache + leading, chksz);

		off += chksz;
		ptr += chksz;
		size -= chksz;

		continue;

	skip_block:
		blkofs = off & erasesize_mask;
		off |= erasesize_mask;
		off += blkofs + 1;
		check_bad = true;
	}

	return 0;
}
