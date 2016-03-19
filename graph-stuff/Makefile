.PHONY: all clean

all: graph-stats

clean:
	rm -f graph-stats

graph-stats: graph-stats.cc
	g++ -O3 -std=c++11 $^ -o $@
