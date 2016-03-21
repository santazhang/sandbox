.PHONY: all clean

CXXFLAGS=-O3 -std=c++11 -I.
PRELUDE=@mkdir -p bin

all: \
	bin/cluster1-random \
	bin/graph-stats \
	bin/graph-adj2txt \
	bin/graph-txt2adj \
	bin/mem-stress-load-graph \
	bin/test_adj_index \
	bin/test_graph_eval

clean:
	rm -rf bin

bin/cluster1-random: cluster1-random.cc graph_utils.h
	$(PRELUDE)
	g++ $(CXXFLAGS) $< -o $@

bin/graph-stats: graph-stats.cc
	$(PRELUDE)
	g++ $(CXXFLAGS) $< -o $@

bin/graph-adj2txt: graph-adj2txt.cc
	$(PRELUDE)
	g++ $(CXXFLAGS) $< -o $@

bin/graph-txt2adj: graph-txt2adj.cc
	$(PRELUDE)
	g++ $(CXXFLAGS) $< -o $@

bin/mem-stress-load-graph: mem-stress-load-graph.cc graph_utils.h
	$(PRELUDE)
	g++ $(CXXFLAGS) $< -o $@

bin/test_adj_index: test_adj_index.cc graph_utils.h
	$(PRELUDE)
	g++ $(CXXFLAGS) $< -o $@

bin/test_graph_eval: test_graph_eval.cc graph_utils.h
	$(PRELUDE)
	g++ $(CXXFLAGS) $< -o $@
