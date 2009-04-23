all: timer3

timer3: timer3.o
	gcc timer3.o -lboost_thread-mt -lboost_system-mt -o timer3

timer3.o: timer3.cpp
	gcc -c timer3.cpp

clean:
	rm -rf timer3.o timer3
