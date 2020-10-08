/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Simple MMIO register R/W macros
 */

#ifndef _MMIO_H_
#define _MMIO_H_

#define reg_read32(addr) 		(*(volatile unsigned int *)(addr))
#define reg_write32(addr, val)		(*(volatile unsigned int *)(addr) = (val))
#define reg_set32(addr, val)		(*(volatile unsigned int *)(addr) |= (val))
#define reg_clr32(addr, val)		(*(volatile unsigned int *)(addr) &= ~(val))
#define reg_clrset32(addr, clr, set)	(*(volatile unsigned int *)(addr) = (*(volatile unsigned int *)(addr) & (~(clr))) | (set))

#endif /* _MMIO_H_ */
