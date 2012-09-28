all: bench-svr bench-clnt

bench-svr: rpc.o bench-svr.o
	gcc rpc.o bench-svr.o -o bench-svr

bench-clnt: rpc.o bench-clnt.o
	gcc rpc.o bench-clnt.o -o bench-clnt

rpc.o: rpc.cc rpc.h
	gcc rpc.cc -c -o rpc.o

clean:
	rm -f *.o bench-clnt bench-svr

