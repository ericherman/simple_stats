#

EXECUTABLE=sstats
SOURCES=main.c ss_options.c simple_stats.c

CC=gcc
STD_CFLAGS=--std=c99
NOISY_CFLAGS=-Werror -Wall -Wextra -pedantic
OPTIMIZER_CFLAGS=-ggdb -O0

CFLAGS=$(STD_CFLAGS) $(OPTIMIZER_CFLAGS) $(NOISY_CFLAGS)

# "-lm" needed by simple_stats.c see also: man 3 sqrt
LDFLAGS=-lm

OBJECTS=$(SOURCES:.c=.o)

# extracted from https://github.com/torvalds/linux/blob/master/scripts/Lindent
LINDENT=indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $^ -o $@

clean:
	rm -rf *.o $(EXECUTABLE) *~

tidy:
	patch -Np1 -i ./pre-tidy.patch
	$(LINDENT) \
		-T FILE \
		-T size_t \
		-T ss_options \
		-T simple_stats \
		*.h *.c
	patch -Rp1 -i ./pre-tidy.patch

check:
	cat ./data.txt
	./sstats --file=./data.txt --skip_rows=1 --skip_cols=1 --channels=4
	cat ./anscombe_quartet.csv
	./sstats --file=./anscombe_quartet.csv --skip_rows=1 --channels=8
