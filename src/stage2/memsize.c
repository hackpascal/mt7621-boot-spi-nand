// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Memory size detection for MIPS platform
 */

#include <types.h>
#include <compiler.h>

static void __nomips16 sync(void)
{
	asm volatile ("sync");
}

uintptr_t get_mem_size(uintptr_t base, uintptr_t limit)
{
	volatile uintptr_t *check_base, *check_ptr;
	uintptr_t size, off, curr, orig_base, orig_ptr;
	int addrbit;

	check_base = (volatile uintptr_t *)base;

	/* Backup original base data */
	sync();
	orig_base = *check_base;
	sync();

	for (addrbit = 3; ; addrbit++) {
		size = (uintptr_t)1 << addrbit;
		if (size >= limit) {
			size = limit;
			break;
		}

		off = base + size;
		check_ptr = (volatile uintptr_t *)off;

		/* Backup original ptr data */
		sync();
		orig_ptr = *check_ptr;

		/* Write differnet value to base then ptr */
		sync();
		*check_base = off;
		sync();
		*check_ptr = ~off;

		/* Fetch value from base after ptr is written */
		sync();
		curr = *check_base;

		/* Check if the value of base got changed */
		if (curr != off)
			break;

		/* Restore value of ptr */
		sync();
		*check_ptr = orig_ptr;
	}

	/* Restore value of base */
	sync();
	*check_base = orig_base;
	sync();

	return size;
}
