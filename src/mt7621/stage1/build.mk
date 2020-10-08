# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>

TEXT_BASE := 0xbe108000

HEAD := $(TARGET)/$(STAGE)/entry.o
COBJS += $(TARGET)/$(STAGE)/target.o
