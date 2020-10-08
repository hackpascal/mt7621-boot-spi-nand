// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Simple timer utilities for MIPS platform
 */

#include <types.h>
#include <div64.h>
#include <compiler.h>

static uint32_t timer_freq;
static uint32_t ticks_hi, ticks_lo;

static uint32_t __nomips16 get_counter(void)
{
	uint32_t val;

	asm volatile ("mfc0\t%0,$9" : "=r"(val));

	return val;
}

uint64_t get_ticks(void)
{
	uint32_t new_ticks_lo = get_counter();

	if (new_ticks_lo <= ticks_lo)
		ticks_hi++;

	ticks_lo = new_ticks_lo;

	return ((uint64_t)ticks_hi << 32) | ticks_lo;
}

static uint64_t tick_to_usec(uint64_t tick)
{
	tick *= 1000000;
	__div64_32(&tick, timer_freq);

	return tick;
}

uint64_t get_timer(uint64_t base)
{
	return tick_to_usec(get_ticks()) - base;
}

void udelay(uint32_t usec)
{
	uint64_t tmo = get_timer(0) + usec;

	while (get_timer(0) < tmo)
		;
}

void set_timer_freq(uint32_t freq)
{
	timer_freq = freq;
}
