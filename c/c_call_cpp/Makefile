all: main

main: main.o foo.o
	g++ main.o foo.o -o main

main.o: main.c
	gcc -c main.c -o main.o

foo.o: foo.h foo.cpp
	g++ -c foo.cpp -o foo.o


clean:
	rm -f *.o main
