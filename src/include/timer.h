/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Simple timer utilities
 */

#ifndef _TIMER_H_
#define _TIMER_H_

#include <types.h>

void set_timer_freq(uint32_t freq);
void udelay(uint32_t usec);

#if STAGE == 2
uint64_t get_ticks(void);
uint64_t get_timer(uint64_t base);
#endif

#endif /* _TIMER_H_ */
