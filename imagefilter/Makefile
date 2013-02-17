# This Makefile is intended for mingw32-make

#PNG_FLAGS=-DUSE_ZLIB -I/opt/local/include -I/usr/local/include -L/opt/local/lib -L/usr/local/lib -lz -lpng

CFLAGS=-O3
LDFLAGS=-lz -lpng

all: test

test: test.c imagefilter.c imagefilter.h
	gcc -O3 -lz -lpng imagefilter.c test.c -o test

clean:
	rm -f test
