/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 */

OUTPUT_ARCH(mips)

ENTRY(_start)

SECTIONS {
	. = 0x00000000;

	. = ALIGN(4);
	.text : {
		__copy_start = .;
		__text_start = .;
		*(.text*)
		__text_end = .;
	}

	. = ALIGN(4);
	.rodata : {
		*(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*)))
	}

	. = ALIGN(4);
	.data : {
		*(.data*)
	}

	. = ALIGN(4);
	.sdata : {
		*(.sdata*)
	}

	. = ALIGN(4);
	__copy_end = .;
	__copy_size = . - __copy_start;

	. = ALIGN(4);
	.bss : {
		__bss_start = .;
		*(.bss.*)
		*(.sbss.*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end = .;
	}

	. = ALIGN(4);
	__prog_end = .;
	__prog_size = . - __copy_start;

	/DISCARD/ : {
		*(.note.gnu.*)
		*(.MIPS.abiflags)
		*(.prgend)
	}

#ifdef STAGE2
	__stage3_offset = PAGESIZE + ALIGN(__copy_size, PAGESIZE);
#endif
}
