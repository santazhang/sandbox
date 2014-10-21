all: run

run: run.cc
	g++ -Ipicojson-1.1.1 -Iyajl-2.1.0/build/yajl-2.1.0/include -Irapidjson-98f8790/include \
		-Lyajl-2.1.0/build/yajl-2.1.0/lib -lyajl_s \
		run.cc -o run

clean:
	rm -f run
