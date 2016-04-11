.PHONY: all clean

all: main main2

main.o: main.cc lib.h

main2.o: main2.cc lib.h

lib.o: lib.cc lib.h

lib.a: lib.o
	ar rcs $@ $^

main: main.o lib.a

main2: main2.o lib.a

clean:
	rm -f *.a *.o main main2
