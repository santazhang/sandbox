MKDIR_P=mkdir -p
RMRF=rm -rf
DEL_RECURSIVE=find . -iname
DEL_RECURSIVE_OPTIONS=-delete
CC=gcc
CXX=g++
CFLAGS=-O3 -pthread -DRELEASE -I/opt/local/include/opencv -Isrc 
CXXFLAGS=-O3 -pthread -DRELEASE -I/opt/local/include/opencv -Isrc 
LDFLAGS=-L/opt/local/lib -lpthread -lcv -lhighgui -lcxcore

default: simpic

simpic:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) simpic.cpp  -o simpic

clean:
	$(RMRF) simpic

