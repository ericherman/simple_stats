# SPDX-License-Identifier: LGPL-3.0-or-later
# Makefile
# Copyright (C) 2014 - 2020 Eric Herman <eric@freesa.org>
# https://github.com/ericherman/simple_stats

SHELL:=/bin/bash
lib_LTLIBRARIES=libsstats.la

include_HEADERS=src/simple_stats.h src/simple_stats_report.h

bin_PROGRAMS=sstats
sstats_SOURCES=\
 $(include_HEADERS) \
 submodules/libecheck/src/eembed.h \
 submodules/libecheck/src/eembed.c \
 src/simple_stats_options.h \
 src/simple_stats_options.c \
 src/simple_stats_report.c \
 src/simple_stats_cli_main.c \
 src/simple_stats.c

CC=gcc
STD_CFLAGS=--std=c99 -pedantic
NOISY_CFLAGS=-Werror -Wall -Wextra
OPTIMIZER_CFLAGS=-ggdb -O2 -DNDEBUG -fomit-frame-pointer

CFLAGS += $(STD_CFLAGS) \
	  $(OPTIMIZER_CFLAGS) \
	  $(NOISY_CFLAGS) \
	  -I./submodules/libecheck/src \
	  -fPIC \
	  -pipe

# "-lm" needed by simple_stats.c see also: man 3 sqrt
LDADD=-lm

OBJECTS=$(sstats_SOURCES:.c=.o)
ECHECK_SRCS=\
 ./submodules/libecheck/src/eembed.h \
 ./submodules/libecheck/src/eembed.c \
 ./submodules/libecheck/src/echeck.h \
 ./submodules/libecheck/src/echeck.c

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
		`find src tests -name '*.h' -o -name '*.c'` \
		./simple_stats_arduino_tests_arduino/*.ino



echeck.o: $(ECHECK_SRCS)
	$(CC) -c $(CFLAGS) -I./submodules/libecheck/src \
		./submodules/libecheck/src/echeck.c \
		-o echeck.o

eembed.o: $(ECHECK_SRCS)
	$(CC) -c $(CFLAGS) -I./submodules/libecheck/src \
		./submodules/libecheck/src/eembed.c \
		-o eembed.o


test-sstats-basic: $(sstats_SOURCES) $(OBJECTS) echeck.o eembed.o \
		tests/test-sstats-basic.c
	$(CC) $(CFLAGS) -I./src -I./tests -I./submodules/libecheck/src \
		eembed.o \
		echeck.o \
		./src/simple_stats.o \
		-o test-sstats-basic \
		tests/test-sstats-basic.c \
		$(LDADD)
	ls -l ./test-sstats-basic

check: test-sstats-basic
	./test-sstats-basic

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

clean:
	rm -rf `cat .gitignore | sed -e 's/#.*//'`
	pushd src && rm -rf `cat ../.gitignore | sed -e 's/#.*//'` && popd
	pushd tests && rm -rf `cat ../.gitignore | sed -e 's/#.*//'` && popd
	pushd simple_stats_arduino_tests_arduino \
		&& rm -rf `cat ../.gitignore | sed -e 's/#.*//'` && popd

spotless:
	git clean -dffx
	git submodule foreach --recursive git clean -dffx
