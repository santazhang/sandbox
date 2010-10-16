CC= mpicc
FLAGS=-O3 -g -Wall

all: cpi.mpi

cpi.mpi: cpi.c
	$(CC) $(FLAGS) cpi.c -o cpi.mpi

clean:
	rm -f cpi.mpi *.o
