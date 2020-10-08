/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 */

#ifndef _TARGET_H_
#define _TARGET_H_

#include <types.h>

uintptr_t get_stage2_offset(void);

void target_init(void);
void target_stage2_pre_setup(uintptr_t loadaddr, uint32_t size);
void target_stage2_post_setup(uintptr_t loadaddr, uint32_t size);
void target_stage2_data_process(uintptr_t dstaddr, uint32_t ca);

#endif /* _TARGET_H_ */
