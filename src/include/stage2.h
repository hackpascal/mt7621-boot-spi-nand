/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Definitions for 2nd-stage headers
 */

#ifndef _STAGE2_H_
#define _STAGE2_H_

#include <types.h>

struct stage2_header {
	uint32_t insn[2];
	uint32_t loadaddr;
	uint32_t size;
};

uintptr_t get_mem_size(uintptr_t base, uintptr_t limit);
void set_meminfo(uint32_t base, uint32_t size);
void set_stage3_offset(uint32_t offset);

#endif /* _STAGE2_H_ */
