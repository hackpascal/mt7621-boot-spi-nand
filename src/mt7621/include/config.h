/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * MediaTek MT7621 related parameters
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define L1_DCACHE_SIZE		(32 << 10)
#define L1_DCACHE_LINESIZE	32

#define L1_ICACHE_SIZE		(32 << 10)
#define L1_ICACHE_LINESIZE	32

#define L2_CACHE_SIZE		(256 << 10)
#define L2_CACHE_LINESIZE	32

#define CMGIC_BASE		0x1fbc0000
#define CMGCR_BASE		0x1fbf8000
#define CMCPC_BASE		0x1fbf0000

#endif /* _CONFIG_H_ */
