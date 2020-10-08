/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Wrapper for MediaTek MT7621 DRAMC binary
 */

#include <config.h>
#include <asm.h>
#include <addrspace.h>
#include <mipsregs.h>

#define DRAMC_BIN_LOADADDR	0xBE108800
#define DRAMC_ACT_SIZE		0x60

	.align	2
	.section	".data.dramc_cpu_freq", "a"
	.type	dramc_cpu_freq, @object
	.globl	dramc_cpu_freq
dramc_cpu_freq:
	.word	0
	.size	dramc_cpu_freq, . - dramc_cpu_freq

	.align	2
	.section	".data.dramc_ddr_freq", "a"
	.type	dramc_ddr_freq, @object
	.globl	dramc_ddr_freq
dramc_ddr_freq:
	.word	0
	.size	dramc_ddr_freq, . - dramc_ddr_freq

	.align	2
	.section	".data.dramc_ddr2_act", "a"
	.type	dramc_ddr2_act, @object
	.globl	dramc_ddr2_act
dramc_ddr2_act:
	.word	0
	.size	dramc_ddr2_act, . - dramc_ddr2_act

	.align	2
	.section	".data.dramc_ddr3_act", "a"
	.type	dramc_ddr3_act, @object
	.globl	dramc_ddr3_act
dramc_ddr3_act:
	.word	0
	.size	dramc_ddr3_act, . - dramc_ddr3_act

	.align	2
	.section	".rodata.dramc_bin", "a"
	.type	dramc_bin, @object
dramc_bin:
	.incbin DRAMC_BIN_FILE
	.equ	dramc_bin_size, . - dramc_bin
	.size	dramc_bin, dramc_bin_size

	.text
	.set	noreorder
	.set	nomips16

LEAFUNC(dramc_init)
	li	$t0, 0xBE00001C
	li	$t1, 0x0
	sw	$t1, 0($t0)

	/* move code to SRAM */
	la	$t0, dramc_bin
	li	$t1, DRAMC_BIN_LOADADDR
	li	$t3, dramc_bin_size

1:
	lw	$t2, 0($t0)
	sw	$t2, 0($t1)
	addiu	$t0, $t0, 4
	addiu	$t1, $t1, 4
	subu	$t3, $t3, 4
	bgtz	$t3, 1b
	nop

	/* Override DDR2 AC timing settings */
	la	$t0, dramc_ddr2_act
	lw	$t0, 0($t0)
	li	$t1, DRAMC_BIN_LOADADDR + 0xc8
	li	$t3, DRAMC_ACT_SIZE

1:
	lw	$t2, 0($t0)
	sw	$t2, 0($t1)
	addiu	$t0, $t0, 4
	addiu	$t1, $t1, 4
	subu	$t3, $t3, 4
	bgtz	$t3, 1b
	nop

	/* Override DDR3 AC timing settings */
	la	$t0, dramc_ddr3_act
	lw	$t0, 0($t0)
	li	$t1, DRAMC_BIN_LOADADDR + 0x60
	li	$t3, DRAMC_ACT_SIZE

1:
	lw	$t2, 0($t0)
	sw	$t2, 0($t1)
	addiu	$t0, $t0, 4
	addiu	$t1, $t1, 4
	subu	$t3, $t3, 4
	bgtz	$t3, 1b
	nop

	/* Set CPU frequency */
	la	$t0, dramc_cpu_freq
	lw	$t0, 0($t0)
	li	$t1, DRAMC_BIN_LOADADDR + 0x20
	sw	$t0, 0($t1)

	/* Set DDR frequency */
	la	$t0, dramc_ddr_freq
	lw	$t0, 0($t0)
	li	$t1, DRAMC_BIN_LOADADDR + 0x24
	sw	$t0, 0($t1)

	/* Set baudrate */
	la	$t0, _baudrate
	lw	$t0, 0($t0)
	li	$t1, DRAMC_BIN_LOADADDR + 0x130
	sw	$t0, 0($t1)

	/* Save return address */
	li	$t0, 0xBE10DFF0
	sw	$ra, 0($t0)

	/* Invoke DRAMC bin */
	li	$t9, 0xBE108800
	jalr	$t9
	nop

	/* Restore return address */
	li	$t0, 0xBE10DFF0
	lw	$ra, 0($t0)

	jr	$ra
	nop
ENDFUNC(dramc_init)
