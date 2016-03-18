.PHONY: all clean

all: hdfs_client

hdfs_client: hdfs_client.o
	g++ $^ -L/home/santa/Downloads/libhdfs3/dist/lib -lhdfs3 -o $@

hdfs_client.o: hdfs_client.cc
	g++ $^ -I/home/santa/Downloads/libhdfs3/dist/include -c -o $@

client:
	rm -f *.o hdfs_client
