all: compile

compile: test-omp

test-omp: test-omp.o
	gcc -o test-omp -fopenmp test-omp.o

test-omp.o: test-omp.c
	gcc -c -fopenmp test-omp.c

clean:
	rm -f *.o
	rm test-omp

