# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>

PROGS := padutil

padutil_OBJS := padutil.o

all: $(PROGS)

$(PROGS):
	$(MAKE) -f Makefile.host PROG=$@ SRCDIR=$(SRCDIR) OUTDIR=$(OUTDIR)/build-$@ INSTALLDIR=$(OUTDIR) OBJS="$($(@F)_OBJS)"
