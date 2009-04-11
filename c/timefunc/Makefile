all: compile

compile: gettimeofday

gettimeofday: gettimeofday.o
	gcc gettimeofday.o -o gettimeofday

gettimeofday.o: gettimeofday.c
	gcc -c gettimeofday.c

clean:
	rm -f *.o
	rm gettimeofday

