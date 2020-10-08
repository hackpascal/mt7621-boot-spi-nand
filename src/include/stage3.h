/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Definitions for 3rd-stage headers and entry-point ABIs
 */

#ifndef _STAGE3_H_
#define _STAGE3_H_

#include <types.h>

#define UBOOT_IH_MAGIC			0x27051956
#define UBOOT_IH_NMLEN			32

#define UBOOT_IH_COMP_NONE		0
#define UBOOT_IH_COMP_LZMA		3

typedef struct uboot_image_header {
	uint32_t ih_magic;
	uint32_t ih_hcrc;
	uint32_t ih_time;
	uint32_t ih_size;
	uint32_t ih_load;
	uint32_t ih_ep;
	uint32_t ih_dcrc;
	uint8_t ih_os;
	uint8_t ih_arch;
	uint8_t ih_type;
	uint8_t ih_comp;
	uint8_t ih_name[UBOOT_IH_NMLEN];
} uboot_image_header_t;

typedef void (*u_boot_entry_point_t)(void);


#define BOOTSTRAP_HDR_MAGIC		0x37540178

typedef struct bootstrap_kernel_header_type {
	uint32_t magic;
	uint32_t length;
	uint32_t load_address;
	uint32_t entry_point;
} bootstrap_kernel_header_t;

typedef void (*kernel_entry_point_t)(uint32_t ramsize);

#endif /* _STAGE3_H_ */
