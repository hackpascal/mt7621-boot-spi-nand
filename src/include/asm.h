/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 */

#ifndef _ASM_H_
#define _ASM_H_

#define FUNC(funcname, fgpr, framesize, rpc) \
	.section	.text. ## funcname, "ax", @progbits; \
	.align	2; \
	.globl	funcname; \
	.type	funcname, @function; \
	.ent	funcname, 0; \
funcname: \
	.frame	fgpr, framesize, rpc

#define LEAFUNC(funcname) \
	FUNC(funcname, $sp, 0, $ra)

#define ENTRY(funcname) \
	FUNC(funcname, $sp, 0, $ra)

#define ENDFUNC(funcname) \
	.end	funcname; \
	.size	funcname, . - funcname

#define EXPORT(symbol) \
		.globl	symbol; \
symbol:

#endif /* _ASM_H_ */
