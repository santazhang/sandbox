all: timer2

timer2: timer2.o
	gcc timer2.o -lboost_thread-mt -lboost_system-mt -o timer2

timer2.o: timer2.cpp
	gcc -c timer2.cpp

clean:
	rm -rf timer2.o timer2
