all: rominfo

rominfo: rominfo.o im.o imbmp.o
	g++ $^ -o $@ -liconv

rominfo.o: rominfo.cpp
	g++ -c -O3 -Wall -I../c-imaging rominfo.cpp

im.o: ../c-imaging/im.c
	gcc -c -O3 -Wall -I../c-imaging ../c-imaging/im.c

imbmp.o: ../c-imaging/imbmp.c
	gcc -c -O3 -Wall -I../c-imaging ../c-imaging/imbmp.c

clean:
	rm -f rominfo *.o
