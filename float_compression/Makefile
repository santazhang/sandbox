.PHONY: all

all: extract_floats

clean:
	rm -f *.o extract_floats *.pb.*

caffe.pb.h caffe.pb.cc: caffe.proto
	protoc caffe.proto --cpp_out=.

caffe.pb.o: caffe.pb.h caffe.pb.cc

extract_floats: caffe.pb.o extract_floats.o
	g++ $^ -lprotobuf -o $@
