CC= gcc
FLAGS=-O3 -g -Wall -lm

all: nbody nbody.omp

nbody: nbody.c
	$(CC) $(FLAGS) nbody.c -o nbody

nbody.omp: nbody.c
	$(CC) $(FLAGS) nbody.c -o nbody.omp -fopenmp

clean:
	rm -f nbody nbody.omp *.o
