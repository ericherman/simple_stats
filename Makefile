SHELL:=/bin/bash
lib_LTLIBRARIES=libsstats.la

include_HEADERS=src/simple_stats.h

bin_PROGRAMS=sstats
sstats_SOURCES=src/main.c src/ss_options.c src/simple_stats.c $(include_HEADERS)

CC=gcc
STD_CFLAGS=--std=c99
NOISY_CFLAGS=-Werror -Wall -Wextra -pedantic
OPTIMIZER_CFLAGS=-ggdb -O0

CFLAGS=$(STD_CFLAGS) $(OPTIMIZER_CFLAGS) $(NOISY_CFLAGS)

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
	patch -Np1 -i ./pre-tidy.patch
	$(LINDENT) \
		-T FILE \
		-T size_t \
		-T ss_options \
		-T simple_stats \
		`find . -name '*.h' -o -name '*.c'`
	patch -Rp1 -i ./pre-tidy.patch

demos: $(bin_PROGRAMS)
	cat tests/data.txt
	./sstats --file=tests/data.txt --skip_rows=1 --skip_cols=1 --channels=4
	@echo ""
	cat tests/anscombe_quartet.csv
	./sstats --file=tests/anscombe_quartet.csv --skip_rows=1 --channels=8

spotless:
	rm -rf `cat .gitignore | sed -e 's/#.*//'`
	pushd src && rm -rf `cat ../.gitignore | sed -e 's/#.*//'` && popd
	pushd tests && rm -rf `cat ../.gitignore | sed -e 's/#.*//'` && popd

check: demos

clean:
	rm -rf *.o $(bin_PROGRAMS) *~
