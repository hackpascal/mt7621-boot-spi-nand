# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>

TOPDIR := $(CURDIR)
export TOPDIR

TARGET_SRCDIR := $(TOPDIR)/src
TOOLS_SRCDIR := $(TOPDIR)/tools
TOP_BUILD_DIR := $(TOPDIR)/build

TOOLS_DIR := $(TOP_BUILD_DIR)/tools
export TOOLS_DIR

all: tools
	$(MAKE) -C $(TARGET_SRCDIR) all SRCDIR=$(TARGET_SRCDIR) OUTDIR=$(TOP_BUILD_DIR) TARGET_NAME=target

tools:
	$(MAKE) -C $(TOOLS_SRCDIR) all SRCDIR=$(TOOLS_SRCDIR) OUTDIR=$(TOOLS_DIR)

clean:
	rm -rf $(TOP_BUILD_DIR)

.PHONY: tools clean
