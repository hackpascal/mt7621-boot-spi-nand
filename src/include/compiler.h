/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Compiler-specific functions
 */

#ifndef _COMPILER_H_
#define _COMPILER_H_

#define __nomips16		__attribute__ ((nomips16))
#define __noreturn		__attribute__ ((noreturn))
#define __maybe_unused		__attribute__ ((unused))

#define unreachable()		__builtin_unreachable()

#endif /* _COMPILER_H_ */
