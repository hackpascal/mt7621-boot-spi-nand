// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Simple delay timer for MIPS platform
 */

#include <types.h>
#include <compiler.h>

static uint32_t timer_freq;

static uint32_t __nomips16 get_ticks(void)
{
	uint32_t val;

	asm volatile ("mfc0\t%0,$9" : "=r"(val));

	return val;
}

void udelay(uint32_t usec)
{
	uint32_t cp0_count;
	uint32_t t_delay, t_expire;

	cp0_count = get_ticks();
	t_delay = usec * (timer_freq / 1000000);

	if (~cp0_count < t_delay) {
		t_expire = t_delay - ~cp0_count;
		while (get_ticks() > t_expire);
	} else {
		t_expire = cp0_count + t_delay;
		while (get_ticks() < t_expire);
	}
}

void set_timer_freq(uint32_t freq)
{
	timer_freq = freq;
}
