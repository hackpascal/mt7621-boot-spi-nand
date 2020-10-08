/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Entry for 2nd-stage
 */

#include <config.h>
#include <asm.h>
#include <addrspace.h>
#include <mipsregs.h>
#include <cache.h>
#include <mt7621_regs.h>

	.text
	.set	nomips16
	.set	noreorder

ENTRY(_start)
EXPORT(stage2_info)
	b	_start_real
	nop

	.org	0x08
EXPORT(_load_addr)
	.word	__copy_start
EXPORT(_load_size)
	.word	__prog_size
EXPORT(_stage3_offset)
#ifdef STAGE3_OFFSET
	.word	STAGE3_OFFSET
#else
	.word	__stage3_offset
#endif
EXPORT(_baudrate)
	.word	BAUDRATE
EXPORT(_unused)
	.word	1
EXPORT(_dramc_cpu_freq)
	.word	880
EXPORT(_dramc_ddr_freq)
	.word	1
EXPORT(_dramc_init_type)
	.word	0
EXPORT(_dramc_ddr2_act_sel)
	.word	2
EXPORT(_dramc_ddr3_act_sel)
	.word	2
EXPORT(_dramc_ddr2_act)
	.word	0, 0, 0, 0, 0, 0, 0, 0
	.word	0, 0, 0, 0, 0, 0, 0, 0
	.word	0, 0, 0, 0, 0, 0, 0, 0
EXPORT(_dramc_ddr3_act)
	.word	0, 0, 0, 0, 0, 0, 0, 0
	.word	0, 0, 0, 0, 0, 0, 0, 0
	.word	0, 0, 0, 0, 0, 0, 0, 0

_start_real:
	/* Clear BSS section */
	la	$a0, __bss_start
	la	$a1, __bss_end

1:	bge	$a0, $a1, 2f
	nop

	sw	$0, 0($a0)
	addiu	$a0, 4
	b	1b
	nop
2:

	/* Setup initial stack */
	li	$sp, 0xbe10dff0

	/* Pre-initialization */
	la	$t9, mt7621_stage2_pre_init
	jalr	$t9
	nop

	/* Set MPLL from CR */
	li	$t0, PALMBUS_BASE
	lw	$t1, (SYSCTL_CLKCFG0 - PALMBUS_BASE)($t0)
	li	$t2, MPLL_CFG_SEL
	or	$t1, $t1, $t2
	sw	$t1, (SYSCTL_CLKCFG0 - PALMBUS_BASE)($t0)

	/* Initialize DRAM & CPU */
	bal	dramc_init
	nop

	/* Set CPU clock from CPLL */
	li	$t0, PALMBUS_BASE
	lw	$t1, (SYSCTL_CLKCFG0 - PALMBUS_BASE)($t0)
	li	$t2, 1
	ins	$t1, $t2, 30, 2
	sw	$t1, (SYSCTL_CLKCFG0 - PALMBUS_BASE)($t0)

	/* Move stage2 code from locked L2-cache to DRAM */
	bal	self_solidify
	nop

	/* Release FE SRAM */
	li	$t0, PALMBUS_BASE
	li	$t0, FE_BASE
	li	$t1, 1
	sw	$t1, FE_RST_GLO($t0)

	/* Setup final stack */
	li	$sp, 0x86000000

	/* Continue to stage2 common routine */
	la	$t9, stage2_main
	jr	$t9
	nop
ENDFUNC(_start)

LEAFUNC(self_solidify)
	la	$a0, __copy_start
	la	$a1, __prog_end

	li	$v1, ~(4 - 1)
	and	$a0, $a0, $v1

	addiu	$a1, 4 - 1
	and	$a1, $a1, $v1

	li	$a2, KSEG1
	ins	$a2, $a0, 0, 29

1:	lw	$a3, 0($a0)
	sw	$a3, 0($a2)
	addiu	$a0, 4
	addiu	$a2, 4
	bne	$a0, $a1, 1b
	nop

	/* Set KSEG0 to Uncached */
	mfc0	$t0, CP0_CONFIG
	ins	$t0, $0, 0, 3
	ori	$t0, $t0, CONF_CM_UNCACHED
	mtc0	$t0, CP0_CONFIG
	ehb

	/* Invalidate L2 Cache */
	move	$a0, $0
	li	$a1, L2_CACHE_SIZE

	mtc0	$0, CP0_STAGLO

1:
	cache	INDEX_STORE_TAG_SD, 0($a0)
	addiu	$a0, $a0, L2_CACHE_LINESIZE
	bne	$a0, $a1, 1b
	nop

	/* Set KSEG0 to Cachable */
	mfc0	$t0, CP0_CONFIG
	ins	$t0, $0, 0, 3
	ori	$t0, $t0, CONF_CM_CACHABLE_COW
	mtc0	$t0, CP0_CONFIG
	ehb

	jr	$ra
	nop
ENDFUNC(self_solidify)
