/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Standard C library implementation
 */

#ifndef _STDDEF_H_
#define _STDDEF_H_

#define NULL ((void *)0)

#define offsetof(TYPE, MEMBER) __builtin_offsetof(TYPE, MEMBER)

#endif /* _STDDEF_H_ */
