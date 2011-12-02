CXFLAG=-ggdb -O3 -I/opt/local/include/opencv -I/opt/local/include -I/usr/include/opencv

all: puzzle-diff

puzzle-diff: compress.o cvec.o dvec.o puzzle-diff.o puzzle.o tunables.o vector_ops.o
	gcc $^ -L/usr/lib -lcv -lcvaux -lcxcore -lhighgui -lm -o $@
#	gcc $^ -L/opt/local/lib -lopencv_core -lopencv_highgui -o $@

all_h: globals.h puzzle.h puzzle_common.h puzzle_p.h

compress.o: compress.c all_h
	gcc $(CXFLAG) compress.c -c -o $@

cvec.o: cvec.c all_h
	gcc $(CXFLAG) cvec.c -c -o $@

dvec.o: dvec.c all_h
	gcc $(CXFLAG) dvec.c -c -o $@

puzzle-diff.o: puzzle-diff.c all_h
	gcc $(CXFLAG) puzzle-diff.c -c -o $@

puzzle.o: puzzle.c all_h
	gcc $(CXFLAG) puzzle.c -c -o $@

tunables.o: tunables.c all_h
	gcc $(CXFLAG) tunables.c -c -o $@

vector_ops.o: vector_ops.c all_h
	gcc $(CXFLAG) vector_ops.c -c -o $@

clean:
	rm -f *.o puzzle-diff
