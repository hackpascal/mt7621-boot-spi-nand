/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Generic cache manipluation for MIPS platform
 */

#include <config.h>
#include <asm.h>
#include <cache.h>
#include <mipsregs.h>

	.text
	.set nomips16
	.set noreorder

#if L1_DCACHE_LINESIZE == L1_ICACHE_LINESIZE
LEAFUNC(invalidate_l1_cache_range)
	li	$v1, ~(L1_DCACHE_LINESIZE - 1)
	add	$a1, $a1, $a0
	addi	$a1, L1_DCACHE_LINESIZE - 1
	and	$a0, $a0, $v1
	and	$a1, $a1, $v1

1:
	cache	HIT_INVALIDATE_I, 0($a0)
	cache	HIT_INVALIDATE_D, 0($a0)
	addi	$a0, L1_DCACHE_LINESIZE
	bne	$a0, $a1, 1b
	nop

	jr	$ra
	nop
ENDFUNC(invalidate_l1_cache_range)

LEAFUNC(flush_l1_cache_range)
	li	$v1, ~(L1_DCACHE_LINESIZE - 1)
	add	$a1, $a1, $a0
	addi	$a1, L1_DCACHE_LINESIZE - 1
	and	$a0, $a0, $v1
	and	$a1, $a1, $v1

1:
	cache	HIT_WRITEBACK_INV_D, 0($a0)
	cache	HIT_INVALIDATE_I, 0($a0)
	addi	$a0, L1_DCACHE_LINESIZE
	bne	$a0, $a1, 1b
	nop

	jr	$ra
	nop
ENDFUNC(flush_l1_cache_range)

#if L1_DCACHE_SIZE == L1_ICACHE_SIZE
LEAFUNC(reset_l1_cache)
	move	$a0, $0
	li	$a1, L1_DCACHE_SIZE

	mtc0	$0, CP0_TAGLO
	mtc0	$0, CP0_TAGHI
	mtc0	$0, CP0_DTAGLO
	mtc0	$0, CP0_DTAGHI

1:
	cache	INDEX_STORE_TAG_I, 0($a0)
	cache	INDEX_STORE_TAG_D, 0($a0)
	addiu	$a0, $a0, L1_DCACHE_LINESIZE
	bne	$a0, $a1, 1b
	nop

	jr	$ra
	nop
ENDFUNC(reset_l1_cache)
#else
LEAFUNC(reset_l1_cache)
	mtc0	$0, CP0_TAGLO
	mtc0	$0, CP0_TAGHI
	mtc0	$0, CP0_DTAGLO
	mtc0	$0, CP0_DTAGHI

	move	$a0, $0
	li	$a1, L1_DCACHE_SIZE
1:
	cache	INDEX_STORE_TAG_D, 0($a0)
	addiu	$a0, $a0, L1_DCACHE_LINESIZE
	bne	$a0, $a1, 1b
	nop

	move	$a0, $0
	li	$a1, L1_ICACHE_SIZE
1:
	cache	INDEX_STORE_TAG_I, 0($a0)
	addiu	$a0, $a0, L1_ICACHE_LINESIZE
	bne	$a0, $a1, 1b
	nop

	jr	$ra
	nop
ENDFUNC(reset_l1_cache)
#endif

#endif /* L1_DCACHE_LINESIZE == L1_ICACHE_LINESIZE */
