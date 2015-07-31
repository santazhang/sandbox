all: run run_O3

FLAGS=-Ipicojson-1.1.1 -Iyajl-2.1.0/build/yajl-2.1.0/include -Irapidjson-98f8790/include \
		-I jsoncpp-7bd75b0/include \
		-I jansson-2.7/build/include \
		run.cc \
		-Lyajl-2.1.0/build/yajl-2.1.0/lib -lyajl_s \
		-L jsoncpp-7bd75b0/build/lib -ljsoncpp \
		-L jansson-2.7/build/lib -ljansson

run: run.cc
	g++ $(FLAGS) -o run
	g++ $(FLAGS) -O3 -o run_O3

clean:
	rm -f run run_O3
