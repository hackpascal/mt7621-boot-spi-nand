// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * 2nd-stage main routine
 */

#include <target.h>
#include <console.h>
#include <spi-nand-load.h>
#include <compiler.h>
#include <byteorder.h>
#include <errno.h>
#include <malloc.h>
#include <stage3.h>
#include <crc32.h>
#include <cache.h>
#include <lzma.h>

static void load_stage3(void);

static uint32_t membase, memsize, stage3_offset;

void __noreturn stage2_main(void)
{
	target_init();

	puts("\n");
	puts("Low-level initialization program for MT7621 [Version 1.0]\n");
	puts("Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>\n");
	printf("DRAM: %uMB\n", memsize >> 20);
	puts("\n");

	malloc_init(membase + memsize);
	make_crc32_table();
	spi_nand_load_init();
	load_stage3();

	while (1);

	unreachable();
}

void set_meminfo(uint32_t base, uint32_t size)
{
	membase = base;
	memsize = size;
}

void set_stage3_offset(uint32_t offset)
{
	stage3_offset = offset;
}

static void load_stage3_breed(bootstrap_kernel_header_t *hdr)
{
	uint32_t size, load_addr, ep, decsize = 0;
	kernel_entry_point_t breed_ep;
	void *payload;
	int ret;

	size = be32_to_cpu(hdr->length);
	load_addr = be32_to_cpu(hdr->load_address);
	ep = be32_to_cpu(hdr->entry_point);

	puts("\n");
	puts("Stage3 is Breed\n");
	printf("Payload size         : 0x%x\n", size);
	printf("Payload load address : 0x%x\n", load_addr);
	printf("Payload entry point  : 0x%x\n", ep);
	puts("\n");

	payload = malloc(size);

	ret = spi_nand_load_skip_bad(stage3_offset + sizeof(*hdr), payload,
				     size);
	spi_nand_read_page_to_cache(0);
	if (ret && ret != -EBADMSG) {
		puts("Failed to load stage3!\n");
		return;
	}

	ret = lzma_uncompress(payload, size, (void *)load_addr, &decsize);
	if (ret) {
		printf("LZMA decompression error %u\n", ret);
		return;
	}

	flush_l1_cache_range(load_addr, decsize);

	breed_ep = (kernel_entry_point_t)ep;
	breed_ep(memsize);

	unreachable();
}

static void load_stage3_u_boot(uboot_image_header_t *hdr)
{
	uint32_t size, load_addr, ep, ocrc, ncrc, decsize = 0;
	u_boot_entry_point_t u_boot_ep;
	void *payload;
	int ret;

	ocrc = be32_to_cpu(hdr->ih_hcrc);
	hdr->ih_hcrc = 0;
	ncrc = crc32(0, hdr, sizeof(*hdr));

	if (ocrc != ncrc) {
		printf("Wrong header CRC. Expect %08x, got %08x\n", ocrc, ncrc);
		return;
	}

	size = be32_to_cpu(hdr->ih_size);
	load_addr = be32_to_cpu(hdr->ih_load);
	ep = be32_to_cpu(hdr->ih_ep);

	puts("\n");
	puts("Stage3 is U-Boot\n");
	printf("Payload size         : 0x%x\n", size);
	printf("Payload load address : 0x%x\n", load_addr);
	printf("Payload entry point  : 0x%x\n", ep);
	puts("\n");

	if (hdr->ih_comp == UBOOT_IH_COMP_NONE) {
		ret = spi_nand_load_skip_bad(stage3_offset + sizeof(*hdr),
					     (void *)load_addr, size);
		spi_nand_read_page_to_cache(0);
		if (ret && ret != -EBADMSG) {
			puts("Failed to load stage3!\n");
			return;
		}

		flush_l1_cache_range(load_addr, size);
	} else if (hdr->ih_comp == UBOOT_IH_COMP_LZMA) {
		payload = malloc(size);

		ret = spi_nand_load_skip_bad(stage3_offset + sizeof(*hdr),
					     payload, size);
		spi_nand_read_page_to_cache(0);
		if (ret && ret != -EBADMSG) {
			puts("Failed to load stage3!\n");
			return;
		}

		ret = lzma_uncompress(payload, size, (void *)load_addr,
				      &decsize);
		if (ret) {
			printf("LZMA decompression error %u\n", ret);
			return;
		}

		flush_l1_cache_range(load_addr, decsize);
	} else {
		printf("Unsupported compression method %u\n", hdr->ih_comp);
		return;
	}

	u_boot_ep = (u_boot_entry_point_t)ep;
	u_boot_ep();

	unreachable();
}

static void load_stage3(void)
{
	int ret;

	union {
		uboot_image_header_t u_boot;
		bootstrap_kernel_header_t breed;
	} hdr;

	ret = spi_nand_load_skip_bad(stage3_offset, &hdr, sizeof(hdr));
	spi_nand_read_page_to_cache(0);
	if (ret && ret != -EBADMSG) {
		puts("Failed to read stage3 header!\n");
		return;
	}

	if (be32_to_cpu(hdr.breed.magic) == BOOTSTRAP_HDR_MAGIC)
		load_stage3_breed(&hdr.breed);
	else if (be32_to_cpu(hdr.u_boot.ih_magic) == UBOOT_IH_MAGIC)
		load_stage3_u_boot(&hdr.u_boot);
	else
		puts("Unrecognized stage3 payload!\n");
}
