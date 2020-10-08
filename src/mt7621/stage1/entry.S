/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Entry for 1st-stage
 */

#include <config.h>
#include <asm.h>
#include <cache.h>
#include <addrspace.h>
#include <mipsregs.h>
#include <mipscmregs.h>
#include <mt7621_regs.h>

	.text
	.set nomips16
	.set noreorder

ENTRY(_start)
	b	_start_real
	nop

	.org	0x08
EXPORT(_stage2_offset)
	.word	PAGESIZE

EXPORT(_baudrate)
	.word	BAUDRATE

_start_real:
	mfc0	$t0, CP0_EBASE
	and	$t0, $t0, EBASE_CPUNUM
	bnez	$t0, _start_real
	nop

	mtc0	$0, CP0_COUNT
	mtc0	$0, CP0_COMPARE

	/* Init CP0 Status */
	mfc0	$t0, CP0_STATUS
	and	$t0, ST0_IMPL
	or	$t0, ST0_BEV | ST0_ERL
	mtc0	$t0, CP0_STATUS
	nop

	/* Clear Watch Status bits and disable watch exceptions */
	li	$t0, 0x7		# Clear I, R and W conditions
	mtc0	$0, CP0_WATCHLO, 0
	mtc0	$t1, CP0_WATCHHI, 0
	mtc0	$0, CP0_WATCHLO, 1
	mtc0	$t1, CP0_WATCHHI, 1
	mtc0	$0, CP0_WATCHLO, 2
	mtc0	$t1, CP0_WATCHHI, 2
	mtc0	$0, CP0_WATCHLO, 3
	mtc0	$t1, CP0_WATCHHI, 3

	/* Clear WP, IV and SW interrupts */
	mtc0	$0, CP0_CAUSE

	/* Set KSEG0 to Uncached */
	mfc0	$t0, CP0_CONFIG
	ins	$t0, $0, 0, 3
	ori	$t0, $t0, CONF_CM_UNCACHED
	mtc0	$t0, CP0_CONFIG
	ehb

	/* Setup GCR base */
1:	mfc0	$t0, CP0_CMGCRBASE
	sll	$t0, $t0, 4

	li	$t1, CMGCR_BASE
	beq	$t0, $t1, 2f

	/* Move the GCRs to our configured base address */
	li	$t2, KSEG1BASE
	addu	$t0, $t0, $t2
	sw	$0, GCR_BASE_UPPER($t0)
	sw	$t1, GCR_BASE($t0)

	/* Re-check the GCR base */
	b	1b
	nop
2:

	/* Setup basic CPS */
	li	$t0, (KSEG1BASE + CMGCR_BASE)
	li	$t1, GCR_REG0_BASE_VALUE
	sw	$t1, (GCR_REGn_BASE(0))($t0)

	li	$t1, ((GCR_REG0_MASK_VALUE << GCR_REGn_MASK_ADDRMASK_S) | \
		     GCR_REGn_MASK_CMTGT_IOCU0)
	sw	$t1, (GCR_REGn_MASK(0))($t0)

	lw	$t1, GCR_BASE($t0)
	ins	$t1, $0, 0, 2
	sw	$t1, GCR_BASE($t0)

	lw	$t1, GCR_CONTROL($t0)
	li	$t2, GCR_CONTROL_SYNCCTL
	or	$t1, $t1, $t2
	sw	$t1, GCR_CONTROL($t0)

	/* Increase SPI frequency */
	li	$t0, PALMBUS_BASE
	li	$t1, 5
	sw	$t1, (SPI_MMAP_SPACE - PALMBUS_BASE)($t0)

	/* Set CPU clock to 500MHz */
	lw	$t1, (SYSCTL_CLKCFG0 - PALMBUS_BASE)($t0)
	ins	$t1, $0, 30, 2
	sw	$t1, (SYSCTL_CLKCFG0 - PALMBUS_BASE)($t0)

	/* Set CPU clock divider to 1/1 */
	li	$t1, 0x101
	sw	$t1, (RBUS_DYN_CFG0 - PALMBUS_BASE)($t0)

	/* Enable Frame Engine SRAM */
	li	$t0, FE_BASE
	li	$t1, 1
	sw	$t1, FE_RST_GLO($t0)
	li	$t1, 6
	sw	$t1, FE_RST_GLO($t0)

	/* Initialize L1 & L2 Cache */
	bal	cache_init
	nop

	/* Copy bootcode into SRAM */
	la	$a0, _start
	li	$a1, 0xbfc00000
	li	$a2, 0x800

1:	lw	$a3, 0($a1)
	sw	$a3, 0($a0)
	addiu	$a0, 4
	addiu	$a1, 4
	sub	$a2, 4
	bnez	$a2, 1b
	nop

	/* Clear BSS section */
	la	$a0, __bss_start
	la	$a1, __bss_end

1:	sw	$0, 0($a0)
	addiu	$a0, 4
	bne	$a0, $a1, 1b
	nop

	/* Setup stack */
	li	$sp, 0xbe10dff0

	/* Continue to run in SRAM */
	la	$t9, stage1_main
	jr	$t9
	nop
ENDFUNC(_start)

LEAFUNC(lock_l2_cache)
	li	$v0, 0x1ffff800
	li	$v1, ~(L2_CACHE_LINESIZE - 1)
	add	$a1, $a1, $a0
	addi	$a1, L2_CACHE_LINESIZE - 1
	and	$a0, $a0, $v1
	and	$a1, $a1, $v1

	mtc0	$0, CP0_SDATALO
	mtc0	$0, CP0_SDATAHI

1:
	and	$v1, $a0, $v0
	ori	$v1, 0xa0
	mtc0	$v1, CP0_STAGLO
	cache	INDEX_STORE_TAG_SD, 0($a0)
	cache	INDEX_STORE_DATA_SD, 0($a0)
	cache	INDEX_STORE_DATA_SD, 8($a0)
	cache	INDEX_STORE_DATA_SD, 16($a0)
	cache	INDEX_STORE_DATA_SD, 24($a0)
	addi	$a0, L2_CACHE_LINESIZE
	bne	$a0, $a1, 1b
	nop

	jr	$ra
	nop
ENDFUNC(lock_l2_cache)

LEAFUNC(fill_l2_cache)
	addiu	$a2, 7
	ins	$a2, $0, 0, 3

1:	lw	$t0, 0($a1)
	mtc0	$t0, CP0_SDATALO
	lw	$t0, 4($a1)
	mtc0	$t0, CP0_SDATAHI
	cache	INDEX_STORE_DATA_SD, 0($a0)
	addiu	$a0, 8
	addiu	$a1, 8
	sub	$a2, 8
	bnez	$a2, 1b
	nop

	jr	$ra
	nop
ENDFUNC(fill_l2_cache)

FUNC(cache_init, $sp, 0, $ra)
	move	$s0, $ra

	li	$s1, (KSEG1BASE + CMGCR_BASE)

	/* Enable CCA override. Set to uncached */
	lw	$s2, 0($s1)
	ins	$s2, $0, 4, 4
	ori	$s3, $s2, (GCR_CCA_DEFAULT_OVERRIDE_ENABLE | (2 << GCR_CCA_DEF_OVERRIDE_VALUE_S))
	sw	$s3, 0($s1)

	/* Initialize L1 Cache */
	bal	reset_l1_cache
	nop

	/* Initialize L2 Cache */
	move	$a0, $0
	li	$a1, L2_CACHE_SIZE

	mtc0	$0, CP0_STAGLO

1:
	cache	INDEX_STORE_TAG_SD, 0($a0)
	addiu	$a0, $a0, L2_CACHE_LINESIZE
	bne	$a0, $a1, 1b
	nop

	/* Dsiable CCA override */
	sw	$s2, 0($s1)

	/* Set KSEG0 to non-coherent cached (important!) */
	mfc0	$t0, CP0_CONFIG
	ins	$t0, $0, 0, 3
	ori	$t0, CONF_CM_CACHABLE_NONCOHERENT
	mtc0	$t0, CP0_CONFIG
	ehb

	/* Initialize L1 Cache again */
	bal	reset_l1_cache
	nop

	/* Disable L2 cache bypass */
	mfc0	$t0, CP0_CONFIG2
	ins	$t0, $0, 12, 1
	mtc0	$t0, CP0_CONFIG2
	ehb

	/* Clear WSC & SPR bit in ErrCtl */
	mfc0	$t0, CP0_ECC
	li	$t1, 0xcfffffff
	and	$t0, $t0, $t1
	mtc0	$t0, CP0_ECC
	ehb

	move	$ra, $s0
	jr	$ra
	nop
ENDFUNC(cache_init)
