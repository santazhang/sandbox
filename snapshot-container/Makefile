all: unittest

test: unittest
	./unittest

unittest: unittest.cc snapshot_container.h
	g++ unittest.cc -o unittest

clean:
	rm -f unittest
