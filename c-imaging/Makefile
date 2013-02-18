CFLAGS=-O3
LDFLAGS=-lz -lpng

all: test

test: test.c im.h im.c imfilter.c imfilter.h impng.h impng.c
	gcc -Wall -O3 -lz -lpng im.c imfilter.c impng.c test.c -o test

clean:
	rm -f test
