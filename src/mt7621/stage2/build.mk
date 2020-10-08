# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>

TEXT_BASE := 0x82000000

HEAD := $(TARGET)/$(STAGE)/entry.o
COBJS += $(TARGET)/$(STAGE)/target.o \
	$(TARGET)/$(STAGE)/cps.o

SOBJS += $(TARGET)/$(STAGE)/dramc/dramc.o

ifeq ($(MT7621_DRAMC_DEBUG),y)
AFLAGS += -DDRAMC_BIN_FILE=\"$(TARGET)/$(STAGE)/dramc/mt7621_stage_sram.bin\"
else
AFLAGS += -DDRAMC_BIN_FILE=\"$(TARGET)/$(STAGE)/dramc/mt7621_stage_sram_noprint.bin\"
endif
