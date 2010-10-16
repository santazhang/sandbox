CC= gcc
FLAGS=-O3 -g -Wall -lm

all: cpi cpi.omp

cpi: cpi.c
	$(CC) $(FLAGS) cpi.c -o cpi

cpi.omp: cpi.c
	$(CC) $(FLAGS) cpi.c -o cpi.omp -fopenmp

clean:
	rm -f cpi cpi.omp *.o
