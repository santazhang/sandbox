.PHONY: all clean

CXXFLAGS=-O3 -std=c++11 -I.
PRELUDE=@mkdir -p bin

all: \
	bin/graph-stats \
	bin/mem-stress-load-graph

clean:
	rm -rf bin

bin/graph-stats: graph-stats.cc
	$(PRELUDE)
	g++ $(CXXFLAGS) $< -o $@

bin/mem-stress-load-graph: mem-stress-load-graph.cc graph_utils.h
	$(PRELUDE)
	g++ $(CXXFLAGS) $< -o $@
