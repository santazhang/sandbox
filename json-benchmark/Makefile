all: run

run: run.cc
	g++ -Ipicojson-1.1.1 -Iyajl-2.1.0/build/yajl-2.1.0/include -Irapidjson-98f8790/include \
		-Lyajl-2.1.0/build/yajl-2.1.0/lib -lyajl_s \
		-I jsoncpp-7bd75b0/include \
		-L jsoncpp-7bd75b0/build/lib -ljsoncpp \
		-I jansson-2.7/build/include \
		-L jansson-2.7/build/lib -ljansson \
		run.cc -o run

clean:
	rm -f run
