# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>

COBJS += $(TARGET)/serial.o \
	$(TARGET)/spi.o

include $(TARGET)/$(STAGE)/build.mk

LDFLAGS += -Wl,-Ttext,$(TEXT_BASE)
CFLAGS += -march=mips32r2 -mtune=1004kc -msoft-float -EL -fno-PIC -fno-PIE -mno-abicalls -mips16
CPPFLAGS += -I$(TARGET)/include -DTEXT_BASE=$(TEXT_BASE)
CROSS_COMPILE ?= mipsel-linux-
