/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Register definitions for MediaTek MT7621
 */

#ifndef _MT7621_REGS_H_
#define _MT7621_REGS_H_

#include <bitops.h>

#define	PALMBUS_BASE			0xbe000000

#define SYSCTL_BASE			(PALMBUS_BASE + 0x000)

#define SYSCTL_SYSCFG0			(PALMBUS_BASE + 0x010)
#define XTAL_MODE_SEL_M			0x1c0
#define XTAL_MODE_SEL_S			6
#define XTAL_MODE_SEL_W			3
#define DRAM_TYPE			BIT(4)

#define SYSCTL_CLKCFG0			(PALMBUS_BASE + 0x02c)
#define CPU_CLK_SEL_M			0xc0000000
#define CPU_CLK_SEL_S			30
#define CPU_CLK_SEL_W			2
#define MPLL_CFG_SEL			BIT(23)

#define SYSCTL_RSTCTL			(PALMBUS_BASE + 0x034)
#define SPI_RST				BIT(18)
#define MC_RST				BIT(2)
#define SYS_RST				BIT(0)

#define SYSCTL_CUR_CLK_STS		(PALMBUS_BASE + 0x044)
#define CUR_CPU_FDIV_M			0x1f00
#define CUR_CPU_FDIV_S			8
#define CUR_CPU_FDIV_W			5
#define CUR_CPU_FFRAC_M			0x1f
#define CUR_CPU_FFRAC_S			0
#define CUR_CPU_FFRAC_W			5

#define SYSCTL_GPIOMODE			(SYSCTL_BASE + 0x60)
#define UART1_MODE_M			0x02

#define RBUS_DYN_CFG0			(PALMBUS_BASE + 0x410)

#define SPI_BASE			(PALMBUS_BASE + 0xb00)

#define SPI_TRANS_REG			(SPI_BASE + 0x00)
#define SPI_ADDR_SIZE_MASK		0x03
#define SPI_ADDR_SIZE_SHIFT		19
#define SPI_MASTER_BUSY			BIT(16)
#define SPI_MASTER_START		BIT(8)

#define SPI_OP_ADDR_REG			(SPI_BASE + 0x04)
#define SPI_DIDO_REG(x)			(SPI_BASE + 0x08 + ((x) << 2))

#define SPI_MASTER_REG			(SPI_BASE + 0x28)
#define RS_SLAVE_SEL_MASK		0x7
#define RS_SLAVE_SEL_SHIFT		29
#define RS_CLK_SEL_MASK			0xfff
#define RS_CLK_SEL_SHIFT		16
#define MORE_BUF_MODE			BIT(2)

#define SPI_MOREBUF_REG			(SPI_BASE + 0x2c)
#define CMD_BIT_CNT_MASK		0x3f
#define CMD_BIT_CNT_SHIFT		24
#define MISO_BIT_CNT_MASK		0x1ff
#define MISO_BIT_CNT_SHIFT		12
#define MOSI_BIT_CNT_MASK		0x1ff
#define MOSI_BIT_CNT_SHIFT		0

#define SPI_CS_POLAR_REG		(SPI_BASE + 0x38)
#define SPI_MMAP_SPACE			(SPI_BASE + 0x3c)

#define UART1_BASE			(PALMBUS_BASE + 0xc00)

#define DRAMC_BASE			(PALMBUS_BASE + 0x5000)

#define DRAMC_CPLL_REG			(DRAMC_BASE + 0x0648)
#define CPLL_PREDIV_M			0x3000
#define CPLL_PREDIV_S			12
#define CPLL_FBDIV_M			0x7f0
#define CPLL_FBDIV_S			4
#define CPLL_FBSEL_M			0x03
#define CPLL_FBSEL_S			1

#define FE_BASE				0xbe100000
#define FE_RST_GLO			0x04

#define GCR_REG0_BASE_VALUE		0x1c000000
#define GCR_REG0_MASK_VALUE		0x0000fc00	/* 64M Palmbus */

#endif /* _MT7621_REGS_H_ */
