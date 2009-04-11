all: compile

compile: helloworld

helloworld: helloworld.o
	dmd helloworld.o

helloworld.o: helloworld.d
	dmd -c helloworld.d

clean:
	rm -f *.o
	rm -f helloworld

