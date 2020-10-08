/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Standard C library implementation (GCC-specific)
 */

#ifndef _STDARG_H_
#define _STDARG_H_

typedef __builtin_va_list va_list;

#define va_start(v, l)	__builtin_va_start(v, l)
#define va_end(v)	__builtin_va_end(v)
#define va_arg(v, l)	__builtin_va_arg(v, l)
#define va_copy(d, s)	__builtin_va_copy(d, s)

#endif /* _STDARG_H_ */
