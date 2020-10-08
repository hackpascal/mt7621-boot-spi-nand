// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Low-level initialization code for MediaTek MT7621
 */

#include <types.h>
#include <mmio.h>
#include <timer.h>
#include <serial.h>
#include <string.h>
#include <stage2.h>
#include <console.h>
#include <spi.h>

#include <cps.h>
#include <addrspace.h>
#include <mt7621_regs.h>
#include <mt7621_ddr_param.h>

struct mt7621_stage2_info {
	struct stage2_header hdr;
	uint32_t stage3_offset;
	uint32_t baudrate;
	uint32_t unused;
	uint32_t dramc_cpu_freq;
	uint32_t dramc_ddr_freq;
	uint32_t dramc_init_type;
	uint32_t dramc_ddr2_act_sel;
	uint32_t dramc_ddr3_act_sel;
	ddr_ac_timing_param_t dramc_ddr2_act;
	ddr_ac_timing_param_t dramc_ddr3_act;
};

extern const struct mt7621_stage2_info stage2_info;
extern uint32_t dramc_cpu_freq;
extern uint32_t dramc_ddr_freq;
extern const ddr_ac_timing_param_t *dramc_ddr2_act;
extern const ddr_ac_timing_param_t *dramc_ddr3_act;

static void set_final_timer_freq(void);
static void config_usb_xhci(void);

static const ddr_ac_timing_param_t ddr2_act[] = {
	[DDR2_512M] = {
		0xAA00AA00, 0xAA00AA00, 0x00000007, 0x22174441,
		0x00000000, 0xF0748661, 0x40001273, 0x9F0A0481,
		0x0304692F, 0x15602842, 0x00008888, 0x88888888,
		0x00000000, 0x00000000, 0x00000000, 0x07100000,
		0x00001B63, 0x00002000, 0x00004000, 0x00006000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
	},
	[DDR2_W9751G6KB_A02_1066_512M] = {
		0xAA00AA00, 0xAA00AA00, 0x00000007, 0x33484584,
		0x00000000, 0xF07486A1, 0x50001273, 0x9F010481,
		0x0304693F, 0x15602842, 0x00008888, 0x88888888,
		0x00000000, 0x00000000, 0x00000010, 0x07100000,
		0x00001F73, 0x00002000, 0x00004000, 0x00006000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
	},
	[DDR2_1024M] = {
		0xAA00AA00, 0xAA00AA00, 0x00000007, 0x22174441,
		0x01000000, 0xF0748661, 0x40001273, 0x9F0F0481,
		0x0304692F, 0x15602842, 0x00008888, 0x88888888,
		0x00000000, 0x00000000, 0x00000000, 0x07100000,
		0x00001B63, 0x00002000, 0x00004000, 0x00006000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
	},
	[DDR2_W971GG6KB25_800_1024M] = {
		0xAA00AA00, 0xAA00AA00, 0x00000007, 0x22174430,
		0x01000000, 0xF0748661, 0x40001273, 0x9F0F0481,
		0x0304692F, 0x15602842, 0x00008888, 0x88888888,
		0x00000000, 0x00000000, 0x00000000, 0x07100000,
		0x00001B63, 0x00002000, 0x00004000, 0x00006000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
	},
	[DDR2_W971GG6KB18_1066_1024M] = {
		0xAA00AA00, 0xAA00AA00, 0x00000007, 0x33484584,
		0x01000000, 0xF07486A1, 0x50001273, 0x9F070481,
		0x0304693F, 0x15602842, 0x00008888, 0x88888888,
		0x00000000, 0x00000000, 0x00000010, 0x07100000,
		0x00001F73, 0x00002000, 0x00004000, 0x00006000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
	}
};

static const ddr_ac_timing_param_t ddr3_act[] = {
	[DDR3_1024M] = {
		0xAA00AA00, 0xAA00AA00, 0x00000007, 0x44694683,
		0x01000000, 0xF07486A1, 0xC287221D, 0x9F060481,
		0x03046948, 0x15602842, 0x00008888, 0x88888888,
		0x00000000, 0x00000000, 0x00000210, 0x07100000,
		0x00001B61, 0x00002040, 0x00004010, 0x00006000,
		0x0C000000, 0x07070000, 0x00000000, 0x00000000,
	},
	[DDR3_1024M_KGD] = {
		0xAA00AA00, 0xAA00AA00, 0x00000007, 0x44694673,
		0x01000000, 0xF07486A1, 0xC287221D, 0x9F050481,
		0x03046948, 0x15602842, 0x00008888, 0x88888888,
		0x00000000, 0x00000000, 0x00000220, 0x07100000,
		0x00001B61, 0x00002040, 0x00004010, 0x00006000,
		0x0C000000, 0x07070000, 0x00000000, 0x00000000,
	},
	[DDR3_2048M] = {
		0xAA00AA00, 0xAA00AA00, 0x00000007, 0x44694683,
		0x01000000, 0xF07486A1, 0xC287221D, 0x9F0F0481,
		0x03046948, 0x15602842, 0x00008888, 0x88888888,
		0x00000000, 0x00000000, 0x00000240, 0x07100000,
		0x00001B61, 0x00002040, 0x00004010, 0x00006000,
		0x0C000000, 0x07070000, 0x00000000, 0x00000000,
	},
	[DDR3_4096M] = {
		0xFF00FF00, 0xFF00FF00, 0x00000007, 0x44694683,
		0x01000000, 0xF07406A1, 0xC287221D, 0x9F060481,
		0x03046923, 0x152f2842, 0x00008888, 0x88888888,
		0x00000000, 0x00000000, 0x00000210, 0x07100000,
		0x00001B61, 0x00002040, 0x00004010, 0x00006000,
		0x0C000000, 0x07070000, 0x000C0000, 0x00000000,
	}
};

static const uint32_t xtal_div_tbl[] = {0, 1, 2, 2};

static uint32_t bus_freq;

void mt7621_stage2_pre_init(void)
{
	uint32_t syscfg0, dram_type, cpu_freq, xtal_freq, xtal_div, cpu_clk_fb;

	mt7621_cps_init();

	serial_init();

	/* DRAM type: 0 for DDR3, 1 for DDR2 */
	syscfg0 = reg_read32(SYSCTL_SYSCFG0);
	dram_type = (syscfg0 & DRAM_TYPE) ? MT7621_DDR2 : MT7621_DDR3;

	/* Base clock used for MPLL */
	switch ((syscfg0 & XTAL_MODE_SEL_M) >> XTAL_MODE_SEL_S) {
	case 0:
	case 1:
	case 2:
		xtal_freq = 20;
		xtal_div = 0;
		break;
	case 3:
	case 4:
	case 5:
		xtal_freq = 40;
		xtal_div = 1;
		break;
	default:
		xtal_freq = 25;
		xtal_div = 0;
	}

	/* Select DRAMC parameters */
	cpu_freq = stage2_info.dramc_cpu_freq;
	if (cpu_freq < 400 || cpu_freq > 1200) {
		printf("CPU clock %uMHz is invalid, restoring to default 880MHz\n",
		       cpu_freq);
		cpu_freq = 880;
	}

	cpu_clk_fb = cpu_freq * (1 << xtal_div_tbl[xtal_div]) / xtal_freq - 1;
	dramc_cpu_freq = 0xc0004802 | (cpu_clk_fb << CPLL_FBDIV_S) |
			 (xtal_div << CPLL_PREDIV_S);

	if (stage2_info.dramc_ddr_freq == 1) {
		if (dram_type == MT7621_DDR3) {
			dramc_ddr_freq = 0x11000000;
		} else {
			puts("1200MHz is not valid for DDR2, restoring to default 800MHz");
		dramc_ddr_freq = 0x31000000;
		}
	} else if (stage2_info.dramc_ddr_freq == 2) {
		dramc_ddr_freq = 0x21000000;
	} else if (stage2_info.dramc_ddr_freq == 3) {
		dramc_ddr_freq = 0x31000000;
	} else if (stage2_info.dramc_ddr_freq == 4) {
		dramc_ddr_freq = 0x41000000;
	} else {
		printf("DDR clock option %u is invalid, restoring to default 800MHz\n",
		       stage2_info.dramc_ddr_freq);
		dramc_ddr_freq = 0x31000000;
	}

	if (stage2_info.dramc_init_type == DDR_INIT_PRE_DEFINED_FIRST) {
		if (stage2_info.dramc_ddr2_act_sel >= __DDR2_ACT_MAX) {
			printf("DDR2 AC timing option %u is invalid, restoring to default 128MB\n",
			       stage2_info.dramc_ddr2_act_sel);
			dramc_ddr2_act = &ddr2_act[DDR2_1024M];
		} else {
			dramc_ddr2_act = &ddr2_act[stage2_info.dramc_ddr2_act_sel];
		}

		if (stage2_info.dramc_ddr3_act_sel >= __DDR3_ACT_MAX) {
			printf("DDR3 AC timing option %u is invalid, restoring to default 256MB\n",
			       stage2_info.dramc_ddr3_act_sel);
			dramc_ddr3_act = &ddr3_act[DDR3_2048M];
		} else {
			dramc_ddr3_act = &ddr3_act[stage2_info.dramc_ddr3_act_sel];
		}
	} else {
		dramc_ddr2_act = &stage2_info.dramc_ddr2_act;
		dramc_ddr3_act = &stage2_info.dramc_ddr3_act;
	}
}

void target_init(void)
{
	uint32_t memsize;

	memsize = get_mem_size(KSEG1, 512 << 20);
	if (memsize == 512 << 20)
		memsize = 448 << 20;

	set_meminfo(KSEG0, memsize);

	set_final_timer_freq();

	/* USB XHCI init */
	config_usb_xhci();

	/* SPI init */
	spi_init();

	/* IMPORTANT: SPI clock must not be greater than 20MHz */
	spi_set_clock(bus_freq, 20000000);

	set_stage3_offset(stage2_info.stage3_offset);
}

static void set_final_timer_freq(void)
{
	uint32_t bs, xtal_sel, clkcfg0, cur_clk, mempll18, dividx, fb;
	uint32_t xtal_clk, xtal_div, ffiv, ffrac, cpu_clk;
	const static uint32_t xtal_div_tbl[] = {0, 1, 2, 2};

	bs = reg_read32(SYSCTL_SYSCFG0);
	clkcfg0 = reg_read32(SYSCTL_CLKCFG0);
	cur_clk = reg_read32(SYSCTL_CUR_CLK_STS);

	xtal_sel = (bs & XTAL_MODE_SEL_M) >> XTAL_MODE_SEL_S;

	if (xtal_sel <= 2)
		xtal_clk = 20 * 1000 * 1000;
	else if (xtal_sel <= 5)
		xtal_clk = 40 * 1000 * 1000;
	else
		xtal_clk = 25 * 1000 * 1000;

	switch ((clkcfg0 & CPU_CLK_SEL_M) >> CPU_CLK_SEL_S) {
	case 0:
		cpu_clk = 500 * 1000 * 1000;
		break;
	case 1:
		mempll18 = reg_read32(DRAMC_CPLL_REG);
		dividx = (mempll18 & CPLL_PREDIV_M) >> CPLL_PREDIV_S;
		fb = (mempll18 & CPLL_FBDIV_M) >> CPLL_FBDIV_S;
		xtal_div = 1 << xtal_div_tbl[dividx];
		cpu_clk = (fb + 1) * xtal_clk / xtal_div;
		break;
	default:
		cpu_clk = xtal_clk;
	}

	ffiv = (cur_clk & CUR_CPU_FDIV_M) >> CUR_CPU_FDIV_S;
	ffrac = (cur_clk & CUR_CPU_FFRAC_M) >> CUR_CPU_FFRAC_S;
	cpu_clk = cpu_clk / ffiv * ffrac;

	bus_freq = cpu_clk / 4;

	set_timer_freq(cpu_clk / 2);
}

static void config_usb_xhci(void)
{
	uint32_t syscfg0;

	syscfg0 = reg_read32(SYSCTL_SYSCFG0);

	switch ((syscfg0 & XTAL_MODE_SEL_M) >> XTAL_MODE_SEL_S) {
	case 0:
	case 1:
	case 2:
		break;
	case 3:
	case 4:
	case 5:
		reg_write32(0xbe1d0784, 0x20201a);
		reg_write32(0xbe1d0c20, 0x80104);
		reg_write32(0xbe1d0c1c, 0x1818181e);
		reg_write32(0xbe1d0c24, 0x1e400000);
		reg_write32(0xbe1d0c38, 0x250073);
		reg_write32(0xbe1d0c40, 0x71004a);
		reg_write32(0xbe1d0b24, 0x140);
		reg_write32(0xbe1d0b10, 0x23800000);
		reg_write32(0xbe1d0b04, 0x20000005);
		reg_write32(0xbe1d0b08, 0x12203200);
		reg_write32(0xbe1d0b2c, 0x1400028);
		reg_write32(0xbe1d0a40, 0xffff0001);
		reg_write32(0xbe1d0a44, 0x60001);
		break;
	default:
		reg_write32(0xbe1d0784, 0x20201a);
		reg_write32(0xbe1d0c20, 0x80004);
		reg_write32(0xbe1d0c1c, 0x18181819);
		reg_write32(0xbe1d0c24, 0x18000000);
		reg_write32(0xbe1d0c38, 0x25004a);
		reg_write32(0xbe1d0c40, 0x48004a);
		reg_write32(0xbe1d0b24, 0x190);
		reg_write32(0xbe1d0b10, 0x1c000000);
		reg_write32(0xbe1d0b04, 0x20000004);
		reg_write32(0xbe1d0b08, 0xf203200);
		reg_write32(0xbe1d0b2c, 0x1400028);
		reg_write32(0xbe1d0a40, 0xffff0001);
		reg_write32(0xbe1d0a44, 0x60001);
	}
}
