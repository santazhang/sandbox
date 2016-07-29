CXX=g++
CXXFLAGS=-std=c++11 -O3 -Wno-c++11-narrowing

.PHONY: all clean

all: \
	build/46.bin \
	build/565.bin

build/%.bin: %.cc common.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf build
