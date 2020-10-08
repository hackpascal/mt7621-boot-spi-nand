# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>

ifeq ($(STAGE3_PAYLOAD),)
ifneq ($(STAGE3_OFFSET),)
CPPFLAGS += -DSTAGE3_OFFSET=$(STAGE3_OFFSET)
endif
endif

COBJS += stage2/main.o \
	stage2/printf.o \
	stage2/console.o \
	stage2/memsize.o \
	stage2/timer.o \
	stage2/div64.o \
	stage2/lshr64.o \
	stage2/malloc.o \
	stage2/spi-nand-load.o \
	stage2/lzma/LzmaDec.o \
	stage2/lzma/lzma.o \
	common/crc32.o

BIN_OUT := $(OUTDIR)/$(STAGE)-pad.bin

all: $(OUTDIR)/$(STAGE)-pad.bin

cmd_page_pad = $(TOOLS_DIR)/padutil -a $(PAGESIZE) $< $@

$(OUTDIR)/$(STAGE)-pad.bin: $(OUTDIR)/$(STAGE).bin FORCE
	$(call if_changed,page_pad)