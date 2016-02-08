.PHONY: all clean

DEVSTUFF_ROOT=$(HOME)/Workspace/devstuff

CXXFLAGS=-std=c++11

all: bench

bench: bench.o bench.pb.o bench.grpc.pb.o
	g++ $^ \
		$(DEVSTUFF_ROOT)/lib/libgrpc++_unsecure.a \
		$(DEVSTUFF_ROOT)/lib/libgrpc_unsecure.a \
		$(DEVSTUFF_ROOT)/lib/libgpr.a \
		$(DEVSTUFF_ROOT)/lib/libprotobuf.a \
		-lpthread -lz -o $@

bench.o: bench.pb.h bench.grpc.pb.h
bench.pb.o: bench.pb.h bench.pb.cc
bench.grpc.pb.o: bench.grpc.pb.h bench.grpc.pb.cc

bench.pb.h bench.pb.cc bench.grpc.pb.h bench.grpc.pb.cc: bench.proto
	protoc --version
	protoc --cpp_out=. bench.proto
	protoc --grpc_out=. --plugin=protoc-gen-grpc=$(DEVSTUFF_ROOT)/bin/grpc_cpp_plugin bench.proto

clean:
	rm -f bench *.o *.pb.*
