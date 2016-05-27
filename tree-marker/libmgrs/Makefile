.PHONY: all clean

all: libmgrs.a

libmgrs.a: mgrs.o polarst.o tranmerc.o ups.o utm.o
	ar rcs $@ $^

run-test: test
	@echo "---"
	@./test

test: test.o
	gcc $^ -L. -lmgrs -o $@

clean:
	rm -f *.o *.a test
