CRUFT_EXT=.log .swp .toc .idx .nav .out .snm .dvi .aux .thm .blg .bbl .synctex.gz

all: max-incr max-incr-test-gen $(patsubst %.tex,%.pdf,$(wildcard *.tex))

clean:
	$(foreach f,$(wildcard *.tex),$(foreach n,$(CRUFT_EXT),rm -f $(patsubst %.tex,%$n,$f);))
	$(foreach n,$(wildcard *.tex),rm -f $(patsubst %.tex,%.pdf,$n);)
	rm -f max-incr
	rm -f max-incr-test-gen


%.pdf: %.tex
	xelatex $<
	$(foreach n,$(CRUFT_EXT),rm -f $(patsubst %.tex,%$n,$<);)


max-incr: max-incr.c
	gcc -O3 max-incr.c -o max-incr

max-incr-test-gen: max-incr-test-gen.c
	gcc -O3 max-incr-test-gen.c -o max-incr-test-gen

