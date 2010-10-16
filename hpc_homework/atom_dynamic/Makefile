#Step 1
	CC= g++
	OPT= -O3
#Step 2
#CC= icc
#OPT= -O3 -ipo -vec-report3 -opt-report-levelmed -opt-report-file<opt.txt>
#OPT= -fast

all:
	$(CC) dpd.cpp -o dpd $(OPT)
	$(CC) dpd.cpp -o dpd.omp -fopenmp $(OPT)

clean:
	rm -f dpd *~
