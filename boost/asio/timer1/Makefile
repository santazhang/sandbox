all: timer1

timer1: timer1.o
	gcc timer1.o -lboost_thread-mt -lboost_system-mt -o timer1

timer1.o: timer1.cpp
	gcc -c timer1.cpp

clean:
	rm -rf timer1.o timer1
