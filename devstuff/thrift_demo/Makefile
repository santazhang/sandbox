CXXFLAGS=-std=c++11

.PHONY: all clean

all: bench

clean:
	rm -rf gen-cpp *.o

bench: bench.o gen-cpp/bench_constants.o gen-cpp/bench_types.o gen-cpp/echo.o
	g++ $^ -lthrift -o $@

bench.o: bench.cc gen-cpp/bench_constants.cpp gen-cpp/bench_types.cpp gen-cpp/echo.cpp gen-cpp/bench_constants.h gen-cpp/bench_types.h gen-cpp/echo.h

gen-cpp/bench_constants.o: gen-cpp/bench_constants.cpp gen-cpp/bench_constants.h

gen-cpp/bench_types.o: gen-cpp/bench_types.cpp gen-cpp/bench_types.h

gen-cpp/echo.o: gen-cpp/echo.cpp gen-cpp/echo.h

gen-cpp/bench_constants.cpp gen-cpp/bench_types.cpp gen-cpp/echo.cpp gen-cpp/bench_constants.h gen-cpp/bench_types.h gen-cpp/echo.h: bench.thrift
	thrift -r --gen cpp:cob_style,templates,moveable_types $^
