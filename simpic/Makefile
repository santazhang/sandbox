MKDIR_P=mkdir -p
RMRF=rm -rf
DEL_RECURSIVE=find . -iname
DEL_RECURSIVE_OPTIONS=-delete
CC=gcc
CXX=g++
CFLAGS=-O3 -pthread -DRELEASE -I/opt/local/include/opencv -I/opt/local/include -Isrc
CXXFLAGS=-O3 -pthread -DRELEASE -I/opt/local/include/opencv -I/opt/local/include -Isrc
LDFLAGS=-L/opt/local/lib -lpthread -lopencv_highgui -lopencv_core -lopencv_imgproc

default: simpic

simpic: simpic.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) simpic.cpp  -o simpic

clean:
	$(RMRF) simpic

