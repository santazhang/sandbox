CXXFLAGS = -I.
CXX = g++
CC = g++

.PHONY: all clean

all: test

test: test.o sg6.o gtest/gtest-all.o

test.o: test.cc sg6.h

sg6.o: sg6.h

clean:
	rm -f gtest/*.o *.o test
