all: compile

compile: showimg

showimg: showimg.o
	g++ showimg.o -o showimg -lhighgui

showimg.o: showimg.cpp
	g++ showimg.cpp -c -I/usr/local/include/opencv

clean:
	rm -f showimg
	rm -f *.o
	rm -f *~

