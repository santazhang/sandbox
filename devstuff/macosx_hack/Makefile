.PHONY: all clean

all: libnuma.a

libnuma.a: numa.o
	ar rcs libnuma.a numa.o

numa.o: numa.h numa.c
	gcc -c numa.c -o numa.o

clean:
	rm -f *.o *.a
