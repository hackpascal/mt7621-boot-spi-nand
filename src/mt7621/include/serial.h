/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Simple UART driver for MediaTek MT7621
 */

#ifndef _SERIAL_H_
#define _SERIAL_H_

void serial_init(void);
void serial_putc(char c);

#endif /* _SERIAL_H_ */
