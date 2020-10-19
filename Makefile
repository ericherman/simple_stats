# SPDX-License-Identifier: LGPL-3.0-or-later
# Makefile
# Copyright (C) 2014 - 2019 Eric Herman <eric@freesa.org>
# https://github.com/ericherman/simple_stats

SHELL:=/bin/bash
lib_LTLIBRARIES=libsstats.la

include_HEADERS=src/simple_stats.h

bin_PROGRAMS=sstats
sstats_SOURCES=src/main.c src/ss_options.c src/simple_stats.c $(include_HEADERS)

CC=gcc
STD_CFLAGS=--std=c99 -pedantic
NOISY_CFLAGS=-Werror -Wall -Wextra
OPTIMIZER_CFLAGS=-ggdb -O2 -DNDEBUG -fomit-frame-pointer

CFLAGS += $(STD_CFLAGS) $(OPTIMIZER_CFLAGS) $(NOISY_CFLAGS) -fPIC -pipe

# "-lm" needed by simple_stats.c see also: man 3 sqrt
LDADD=-lm

OBJECTS=$(sstats_SOURCES:.c=.o)

# extracted from https://github.com/torvalds/linux/blob/master/scripts/Lindent
LINDENT=indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0

all: $(sstats_SOURCES) $(bin_PROGRAMS)

$(bin_PROGRAMS): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LDADD)

.c.o:
	$(CC) -c $(CFLAGS) $^ -o $@


tidy:
	$(LINDENT) \
		-T FILE \
		-T size_t \
		-T ss_options \
		`find . -name '*.h' -o -name '*.c'`

test-sstats-basic: $(sstats_SOURCES) $(OBJECTS)
	$(CC) $(CFLAGS) -I./src -I./tests \
		-o test-sstats-basic \
		tests/test-sstats-basic.c \
		$(LDADD)
	./test-sstats-basic

check: test-sstats-basic

demos: $(bin_PROGRAMS)
	@echo ""
	cat tests/data.txt
	@echo ""
	./sstats --file=tests/data.txt --skip_rows=1 --skip_cols=1 --channels=4
	@echo ""
	cat tests/anscombe_quartet.csv
	@echo ""
	./sstats --file=tests/anscombe_quartet.csv --skip_rows=1 --channels=8

# alias
demo: demos

#alias
test: check

spotless:
	rm -rf `cat .gitignore | sed -e 's/#.*//'`
	pushd src && rm -rf `cat ../.gitignore | sed -e 's/#.*//'` && popd
	pushd tests && rm -rf `cat ../.gitignore | sed -e 's/#.*//'` && popd

clean:
	rm -rf *.o src/*.o $(bin_PROGRAMS) *~ src/*~ tests/*~ ./test-sstats-basic
