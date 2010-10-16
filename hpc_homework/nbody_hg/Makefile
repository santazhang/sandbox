CC= g++ -O2 -g
objs=nbody.o bodysystem.o time.o
gens=generate.o bodysystem.o time.o

all: nbody gen

nbody : $(objs) Makefile
	$(CC) $(objs) -o nbody
nbody.o : nbody.cpp
	$(CC) -c nbody.cpp
time.o : time.cpp time.h
	$(CC) -c time.cpp
bodysystem.o : bodysystem.h bodysystem.cpp
	$(CC) -c bodysystem.cpp

gen   : $(gens) Makefile
	$(CC) $(gens) -o gen

clean:
	rm -f $(objs) nbody gen
