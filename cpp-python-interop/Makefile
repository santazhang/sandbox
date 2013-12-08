all: tinymathpy.so cctest

tinymath.o: tinymath.cc tinymath.h
	g++ -c tinymath.cc -pthread -lpthread

tinymath-py.o: tinymath-py.cc
	g++ -fPIC -c $(shell python-config --includes) tinymath-py.cc

tinymathpy.so: tinymath.o tinymath-py.o
	g++ -fPIC -shared $(shell python-config --libs) tinymath-py.o tinymath.o -o tinymathpy.so

cctest: tinymath.o tinymath.h cctest.cc
	g++ tinymath.o cctest.cc -o cctest

clean:
	rm -f tinymathpy.so cctest *.o
