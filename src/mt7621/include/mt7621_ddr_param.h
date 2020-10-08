/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * DRAMC related definitions for MediaTek MT7621
 */

#ifndef _MT7621_DDR_PARAM_H_
#define _MT7621_DDR_PARAM_H_

#include <types.h>

typedef uint32_t ddr_ac_timing_param_t[24];

typedef enum ddr2_act_type {
	DDR2_512M = 0,
	DDR2_W9751G6KB_A02_1066_512M,
	DDR2_1024M,
	DDR2_W971GG6KB25_800_1024M,
	DDR2_W971GG6KB18_1066_1024M,

	__DDR2_ACT_MAX
} ddr2_act_t;

typedef enum ddr3_act_type {
	DDR3_1024M = 0,
	DDR3_1024M_KGD,
	DDR3_2048M,
	DDR3_4096M,

	__DDR3_ACT_MAX
} ddr3_act_t;

typedef enum ddr_init_type {
	DDR_INIT_PRE_DEFINED_FIRST = 0,
	DDR_INIT_BOARD_PROVIDED_FIRST
} ddr_init_type_t;

typedef enum ddr_type {
	MT7621_DDR3 = 0,
	MT7621_DDR2 = 1
} ddr_type_t;

#endif /* _MT7621_DDR_PARAM_H_ */
