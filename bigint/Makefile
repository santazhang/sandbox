CFLAGS=-O3
LDFLAGS=

all: test

test: test.c bi.h bi.c
	gcc -Wall -O3 -lz -lpng bi.c test.c -o test

clean:
	rm -f test
