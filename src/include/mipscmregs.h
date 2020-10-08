/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Register definitions for MIPS CM/CPS
 */

#ifndef _MIPSCMREGS_H_
#define _MIPSCMREGS_H_

/*
 ****************************************************************
 * Global Control Registers
 ****************************************************************
 */

#define GCR_CONFIG			0x0000
#define GCR_CONFIG_NUMIOCU_M		0xff00
#define GCR_CONFIG_NUMIOCU_S		8
#define GCR_CONFIG_PCORES_M		0xff
#define GCR_CONFIG_PCORES_S		0

#define GCR_BASE			0x0008
#define GCR_CCA_DEF_OVERRIDE_VALUE_M	0xe0
#define GCR_CCA_DEF_OVERRIDE_VALUE_S	5
#define GCR_CCA_DEFAULT_OVERRIDE_ENABLE	(1 << 4)
#define GCR_CM_DEFAULT_TARGET_M		0x03
#define GCR_CM_DEFAULT_TARGET_S		0
#define GCR_CM_DEFAULT_TARGET_MEMORY	0

#define GCR_BASE_UPPER			0x000c

#define GCR_CONTROL			0x0010
#define GCR_CONTROL_SYNCCTL		0x10000

#define GCR_ACCESS			0x0020

#define GCR_GIC_BASE			0x0080
#define GCR_GIC_BASE_ADDR_M		0xfffe000
#define GCR_GIC_BASE_ADDR_S		17
#define GCR_GIC_EN			0x01

#define GCR_CPC_BASE			0x0088
#define GCR_CPC_BASE_ADDR_M		0xffff8000
#define GCR_CPC_BASE_ADDR_S		15
#define GCR_CPC_EN			0x01

#define GCR_REGn_BASE(n)		(0x0090 + ((n) << 4))
#define GCR_REGn_MASK(n)		(0x0098 + ((n) << 4))
#define GCR_REGn_MASK_ADDRMASK_S	16
#define GCR_REGn_MASK_CMTGT_IOCU0	0x2

#define GCR_GIC_STATUS			0x00d0
#define GCR_GIC_EX			0x01

#define GCR_CPC_STATUS			0x00f0
#define GCR_CPC_EX			0x01


/*
 ****************************************************************
 * Global Interrupt Controller Registers
 ****************************************************************
 */

#define GIC_SH_CONFIG			0x0000
#define GIC_SH_NUMINTERRUPTS_M		0xff0000
#define GIC_SH_NUMINTERRUPTS_S		16

#define GIC_SH_POL_BASE			0x0100
#define GIC_SH_TRIG_BASE		0x0180
#define GIC_SH_RMASK_BASE		0x0300
#define GIC_SH_SMASK_BASE		0x0380
#define GIC_SH_MASK_BASE		0x0400
#define GIC_SH_PEND_BASE		0x0480

#define GIC_SH_REG(r, i)		((r) + ((i) / 32) * 4)
#define GIC_SH_BIT(i)			((i) % 32)

#define GIC_SH_MAP_PIN_BASE		0x0500
#define GIC_SH_MAP_PIN(i)		(GIC_SH_MAP_PIN_BASE + (i) * 4)
#define GIC_MAP_TO_PIN			0x80000000
#define GIC_MAP_PIN_M			0x3f
#define GIC_MAP_PIN_S			0

#define GIC_SH_MAP_VPE_BASE		0x2000
#define GIC_SH_MAP_VPE(i)		(GIC_SH_MAP_VPE_BASE + (i) * 0x20)

#endif /* _MIPSCMREGS_H_ */
