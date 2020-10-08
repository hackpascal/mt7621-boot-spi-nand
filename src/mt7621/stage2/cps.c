// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * MIPS CPS initialization for MediaTek MT7621
 */

#include <config.h>
#include <types.h>
#include <mmio.h>
#include <mipscmregs.h>
#include <addrspace.h>
#include <genfuncs.h>

static const struct cm_region {
	uint32_t base;
	uint32_t size;
} cm_regions[] = {
	{ 0x1c000000, 0x04000000 },
	{ 0x60000000, 0x10000000 },
	{ 0x1c000000, 0x04000000 },
	{ 0x1c000000, 0x04000000 },
};

static const struct gic_map_pin {
	uint32_t src;
	uint32_t pin;
} gic_map_pins[] = {
	{ 0, 0 },
	{ 1, 0 },
	{ 2, 4 },
	{ 3, 3 },
	{ 4, 0 },
	{ 5, 5 },
	{ 56, 1 },
	{ 57, 1 },
	{ 58, 1 },
	{ 59, 1 },
	{ 60, 2 },
	{ 61, 2 },
	{ 62, 2 },
	{ 63, 2 },
};

static inline uint32_t cmgcr_read(uint32_t reg)
{
	return reg_read32(KSEG1ADDR(CMGCR_BASE) + reg);
}

static inline void cmgcr_write(uint32_t reg, uint32_t val)
{
	reg_write32(KSEG1ADDR(CMGCR_BASE) + reg, val);
}

static inline void cmgcr_rmw(uint32_t reg, uint32_t clr, uint32_t set)
{
	uint32_t val;

	val = cmgcr_read(reg);
	val &= ~clr;
	val |= set;
	cmgcr_write(reg, val);
}

static inline uint32_t cmgic_read(uint32_t reg)
{
	return reg_read32(KSEG1ADDR(CMGIC_BASE) + reg);
}

static inline void cmgic_write(uint32_t reg, uint32_t val)
{
	reg_write32(KSEG1ADDR(CMGIC_BASE) + reg, val);
}

static inline void cmgic_rmw(uint32_t reg, uint32_t clr, uint32_t set)
{
	uint32_t val;

	val = cmgic_read(reg);
	val &= ~clr;
	val |= set;
	cmgic_write(reg, val);
}

static void cm_init(void)
{
	uint32_t num_cores, num_iocu;
	int i;

	num_cores = ((cmgcr_read(GCR_CONFIG) & GCR_CONFIG_PCORES_M) >> GCR_CONFIG_PCORES_S) + 1;

	cmgcr_write(GCR_ACCESS, (1 << num_cores) - 1);

	num_iocu = ((cmgcr_read(GCR_CONFIG) & GCR_CONFIG_NUMIOCU_M) >> GCR_CONFIG_NUMIOCU_S) + 1;

	if (!num_iocu)
		return;

	for (i = 0; i < ARRAY_SIZE(cm_regions); i++) {
		cmgcr_write(GCR_REGn_BASE(i), cm_regions[i].base);
		cmgcr_write(GCR_REGn_MASK(i), (~(cm_regions[i].size - 1)) | GCR_REGn_MASK_CMTGT_IOCU0);
	}

	cmgcr_rmw(GCR_BASE, GCR_CM_DEFAULT_TARGET_M, GCR_CM_DEFAULT_TARGET_MEMORY);
	cmgcr_rmw(GCR_CONTROL, 0, GCR_CONTROL_SYNCCTL);
}

static void cpc_init(void)
{
	if (cmgcr_read(GCR_CPC_STATUS) & GCR_CPC_EX)
		cmgcr_write(GCR_CPC_BASE, CMCPC_BASE | GCR_CPC_EN);
}

static void gic_init(void)
{
	int i;

	if (!(cmgcr_read(GCR_GIC_STATUS) & GCR_GIC_EX))
		return;

	cmgcr_write(GCR_GIC_BASE, CMGIC_BASE | GCR_GIC_EN);

	/* Interrupt 0..5: Level Trigger, Active High */
	cmgic_rmw(GIC_SH_TRIG_BASE, 0x3f, 0);
	cmgic_rmw(GIC_SH_RMASK_BASE, 0, 0x3f);
	cmgic_rmw(GIC_SH_POL_BASE, 0, 0x3f);
	cmgic_rmw(GIC_SH_SMASK_BASE, 0, 0x3f);

	/* Interrupt 56..63: Edge Trigger, Rising Edge */
	/* Hardcoded to set up the last 8 external interrupts for IPI. */
	cmgic_rmw(GIC_SH_REG(GIC_SH_TRIG_BASE, 56), 0, 0xff << GIC_SH_BIT(56));
	cmgic_rmw(GIC_SH_REG(GIC_SH_RMASK_BASE, 56), 0, 0xff << GIC_SH_BIT(56));
	cmgic_rmw(GIC_SH_REG(GIC_SH_POL_BASE, 56), 0, 0xff << GIC_SH_BIT(56));
	cmgic_rmw(GIC_SH_REG(GIC_SH_SMASK_BASE, 56), 0, 0xff << GIC_SH_BIT(56));

	/* Map interrupt source to interrupt pin */
	for (i = 0; i < ARRAY_SIZE(gic_map_pins); i++)
		cmgic_write(GIC_SH_MAP_PIN(gic_map_pins[i].src), GIC_MAP_TO_PIN | gic_map_pins[i].pin);

	/* Interrupt 31..0 map to VPE0 */
	for (i = 0; i < 32; i++)
		cmgic_write(GIC_SH_MAP_VPE(i), 1);

	/*
	 * Direct GIC_int 56..63 to vpe 0..3
	 * MIPS Linux convention that last 16 interrupts implemented be set
	 * aside for IPI signaling.
	 * The actual interrupts are tied low and software sends interrupts
	 * via GIC_SH_WEDGE writes.
	 */
	for (i = 0; i < 4; i++) {
		cmgic_write(GIC_SH_MAP_VPE(56 + i), 1 << i);
		cmgic_write(GIC_SH_MAP_VPE(60 + i), 1 << i);
	}
}

void mt7621_cps_init(void)
{
	cm_init();
	cpc_init();
	gic_init();
}
