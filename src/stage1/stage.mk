# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>

CFLAGS += -flto

CPPFLAGS += -DPAGESIZE=$(PAGESIZE) -DBLOCKSIZE=$(BLOCKSIZE)

COBJS += stage1/main.o \
	stage1/timer.o \
	stage1/console.o \
	stage1/spi-nand.o

BIN_OUT := $(OUTDIR)/$(STAGE)-pad.bin

all: $(OUTDIR)/$(STAGE)-pad.bin

cmd_page_pad = $(TOOLS_DIR)/padutil -a $(PAGESIZE) -m $(PAGESIZE) $< $@

$(OUTDIR)/$(STAGE)-pad.bin: $(OUTDIR)/$(STAGE).bin FORCE
	$(call if_changed,page_pad)
