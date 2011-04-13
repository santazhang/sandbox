all: sol3.pdf sol6.pdf max-incr max-incr-test-gen

clean:
	python clean.py
	rm -f max-incr
	rm -f max-incr-test-gen

sol3.pdf: sol3.tex
	xelatex sol3.tex

sol6.pdf: sol6.tex
	xelatex sol6.tex

max-incr: max-incr.c
	gcc -O3 max-incr.c -o max-incr

max-incr-test-gen: max-incr-test-gen.c
	gcc -O3 max-incr-test-gen.c -o max-incr-test-gen

