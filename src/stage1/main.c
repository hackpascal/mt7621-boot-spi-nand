// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * 1st-stage main routine
 */

#include <target.h>
#include <console.h>
#include <spi-nand.h>
#include <compiler.h>
#include <stage2.h>

static void load_stage2(void);

void __noreturn stage1_main(void)
{
	target_init();

	puts("Bootcode for Micron SPI-NAND [Version 1.0]\n");
	puts("Copyright (C) 2019 Weijie Gao <hackpascal@gmail.com>\n");
	puts("\n");

	spi_nand_init();
	load_stage2();

	unreachable();
}

static void load_stage2(void)
{
	struct stage2_header s2h;
	uint32_t addr, end, pa, ca;
	uintptr_t dstaddr;

	addr = get_stage2_offset() & ~(PAGESIZE - 1);

	spi_nand_addr_to_pa_ca(addr, &pa, &ca);
	spi_nand_read_page_to_cache(pa);

	spi_nand_read_page_cache(ca, &s2h, sizeof(s2h));

	target_stage2_pre_setup(s2h.loadaddr, s2h.size);

	dstaddr = s2h.loadaddr;
	end = (addr + s2h.size + PAGESIZE - 1) & ~(PAGESIZE - 1);

	do {
		target_stage2_data_process(dstaddr, ca);

		dstaddr += PAGESIZE;
		addr += PAGESIZE;
		if (addr >= end)
			break;

		spi_nand_addr_to_pa_ca(addr, &pa, &ca);
		spi_nand_read_page_to_cache(pa);
	} while (1);

	spi_nand_read_page_to_cache(0);

	target_stage2_post_setup(s2h.loadaddr, s2h.size);
}
