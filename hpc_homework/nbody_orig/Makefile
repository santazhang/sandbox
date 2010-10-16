CC= g++ -O2 -g
objs=nbody.o bodysystem.o time.o

nbody : $(objs) Makefile
	$(CC) $(objs) -o nbody
nbody.o : nbody.cpp
	$(CC) -c nbody.cpp
time.o : time.cpp time.h
	$(CC) -c time.cpp
bodysystem.o : bodysystem.h bodysystem.cpp
	$(CC) -c bodysystem.cpp

clean:
	-rm $(objs) nbody
