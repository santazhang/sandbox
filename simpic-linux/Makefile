# This Makefile is intended for mingw32-make

all: calc_fp

calc_fp: calc_fp.o
	gcc -O3 calc_fp.o -lpuzzle -lgd -o calc_fp

calc_fp.o: calc_fp.c
	gcc -O3 -c calc_fp.c

clean:
	rm -rf calc_fp
