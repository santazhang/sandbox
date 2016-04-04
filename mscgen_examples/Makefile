.PHONY: all clean

%.png : %.txt
	mscgen -T png $<

%.svg : %.txt
	mscgen -T svg $<

all: eg1.png eg2.png eg3.png eg4.png eg5.png \
	 eg1.svg eg2.svg eg3.svg eg4.svg eg5.svg

clean:
	rm -f *.png *.svg
