.PHONY: all

all: extract_floats

clean:
	rm -f *.o extract_floats *.pb.*

caffe.pb.h caffe.pb.cc: caffe.proto
	protoc caffe.proto --cpp_out=.

caffe.pb.o: caffe.pb.h caffe.pb.cc

nn_lossy.o: nn_lossy.h nn_lossy.cc

extract_floats: caffe.pb.o nn_lossy.o extract_floats.o
	g++ -O3 $^ -lprotobuf -o $@
