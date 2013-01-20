CC	= gcc

all: linear_hash


linear_hash: main.o linear_hash.o
	$(CC) main.o linear_hash.o -o linear_hash

linear_hash.o: linear_hash.c linear_hash.h
	$(CC) linear_hash.c -c

clean:
	rm -f *.o *~ linear_hash

