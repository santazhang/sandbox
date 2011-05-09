# Makefile to manage tex files

CRUFT_EXT=.log .swp .toc .idx .nav .out .snm .dvi .aux .thm .blg .bbl .synctex.gz

all: $(patsubst %.tex,%.pdf,$(wildcard hw?.tex)) book.pdf

%.pdf: %.tex
	xelatex $<
	$(foreach n,$(CRUFT_EXT),rm -f $(patsubst %.tex,%$n,$<);)

clean:
	$(foreach f,$(wildcard *.tex),$(foreach n,$(CRUFT_EXT),rm -f $(patsubst %.tex,%$n,$f);))
	$(foreach n,$(wildcard *.tex),rm -f $(patsubst %.tex,%.pdf,$n);)

