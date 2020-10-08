/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Generic MIPS Co-processor registers
 */

#ifndef _MIPSREGS_H_
#define _MIPSREGS_H_

#define CP0_COUNT			$9
#define CP0_COMPARE			$11
#define CP0_STATUS			$12
#define CP0_CAUSE			$13
#define CP0_EBASE			$15, 1
#define CP0_CMGCRBASE			$15, 3
#define CP0_CONFIG			$16
#define CP0_CONFIG2			$16, 2
#define CP0_WATCHLO			$18
#define CP0_WATCHHI			$19
#define CP0_ECC				$26
#define CP0_TAGLO			$28
#define CP0_DTAGLO			$28, 2
#define CP0_STAGLO			$28, 4
#define CP0_SDATALO			$28, 5
#define CP0_TAGHI			$29
#define CP0_DTAGHI			$29, 2
#define CP0_SDATAHI			$29, 5

/* CP0_STATUS fields */
#define ST0_IMPL			(3 << 16)
#define ST0_BEV				0x00400000
#define ST0_ERL				0x00000004

/* CP0_EBASE fields */
#define EBASE_CPUNUM			0x3ff

/* CP0_CONFIG fields */
#define CONF_CM_UNCACHED		2
#define CONF_CM_CACHABLE_NONCOHERENT	3
#define CONF_CM_CACHABLE_COW		5

#endif /* _MIPSREGS_H_ */
