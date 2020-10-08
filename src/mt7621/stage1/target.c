// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 */

#include <types.h>
#include <mmio.h>
#include <timer.h>
#include <serial.h>
#include <compiler.h>
#include <cache.h>
#include <spi-nand.h>
#include <spi.h>

extern void __nomips16 lock_l2_cache(uintptr_t base, uint32_t size);
extern void __nomips16 fill_l2_cache(uintptr_t dst, void *src, uint32_t size);

extern uintptr_t _stage2_offset;

uintptr_t get_stage2_offset(void)
{
	return _stage2_offset;
}

void target_init(void)
{
	set_timer_freq(500000000 / 2);
	serial_init();
	spi_init();
}

void target_stage2_pre_setup(uintptr_t loadaddr, uint32_t size)
{
	lock_l2_cache(loadaddr, size);
}

void target_stage2_post_setup(uintptr_t loadaddr, uint32_t size)
{
	invalidate_l1_cache_range(loadaddr, size);

	((void (*)(void))loadaddr)();
}

void target_stage2_data_process(uintptr_t dstaddr, uint32_t ca)
{
	uint8_t buf[PAGESIZE];

	spi_nand_read_page_cache(ca, buf, sizeof(buf));
	fill_l2_cache(dstaddr, buf, PAGESIZE);
}
