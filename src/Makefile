# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>

include $(TOPDIR)/Makefile.build

BAUDRATE ?= 115200
export BAUDRATE

ifeq ($(_4K_PAGE_SUPPORT),1)
PAGESIZE := 0x1000
BLOCKSIZE := 0x40000
else
PAGESIZE := 0x800
BLOCKSIZE := 0x20000
endif

export PAGESIZE BLOCKSIZE

BIN_OUT := $(OUTDIR)/$(TARGET_NAME)/output-loader.bin

DEPS := $(addsuffix .dep,$(BIN_OUT))

STAGES := stage1 stage2

all: $(BIN_OUT)

cmd_cat_final = cat $(OUTDIR)/$(TARGET_NAME)/stage1/stage1-pad.bin $(OUTDIR)/$(TARGET_NAME)/stage2/stage2-pad.bin $(STAGE3_PAYLOAD) > $@

$(BIN_OUT): $(STAGES) FORCE
	$(call if_changed,cat_final)

$(STAGES):
	$(MAKE) -f Makefile.target all STAGE=$@ TARGET=mt7621 SRCDIR=$(SRCDIR) OUTDIR=$(OUTDIR)/$(TARGET_NAME)/$@

.PHONY: $(STAGES)

-include $(DEPS)
