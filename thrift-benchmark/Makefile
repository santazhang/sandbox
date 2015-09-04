all: bin/bm-encode-decode

clean:
	rm -rf gen obj bin

gen/messages_types.h: messages.thrift
	thrift1 -out gen --gen cpp:json,templates=only messages.thrift

gen/echo_types.h: echo.thrift
	thrift1 -out gen --gen cpp:json,templates=only echo.thrift

bin/bm-encode-decode: gen/messages_types.h gen/echo_types.h
