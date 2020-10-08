/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Address space related for MIPS platform
 */

#ifndef _ADDRSPACE_H_
#define _ADDRSPACE_H_

#define PHYSMASK			0x1fffffff

#define KSEG0BASE			0x80000000
#define KSEG1BASE			0xa0000000
#define KSEG2BASE			0xc0000000

#define KSEG0				KSEG0BASE
#define KSEG1				KSEG1BASE
#define KSEG2				KSEG2BASE

#define CPHYSADDR(x)			((x) & PHYSMASK)
#define KSEG1ADDR(x)			(CPHYSADDR(x) | KSEG1BASE)

#endif /* _ADDRSPACE_H_ */
