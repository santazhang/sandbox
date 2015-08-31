#!/bin/bash

. activate.sh

thrift1 -out . --gen cpp:json,templates=only messages.thrift
thrift1 -out . --gen cpp:json,templates=only echo.thrift

BUILD_FLAGS="-O3 -std=c++0x -I /home/santa/fbthrift"

g++ ${BUILD_FLAGS} -c messages_constants.cpp
g++ ${BUILD_FLAGS} -c messages_reflection.cpp
g++ ${BUILD_FLAGS} -c messages_types.cpp
g++ ${BUILD_FLAGS} -c bm-encode-decode.cc
g++ ${BUILD_FLAGS} -c echo-server.cc
g++ ${BUILD_FLAGS} -c echo-client.cc
g++ ${BUILD_FLAGS} -c Echo_server.skeleton.cpp
g++ ${BUILD_FLAGS} -c Echo.cpp
g++ ${BUILD_FLAGS} -c echo_constants.cpp
g++ ${BUILD_FLAGS} -c echo_reflection.cpp
g++ ${BUILD_FLAGS} -c echo_types.cpp

gcc bm-encode-decode.o messages_constants.o messages_reflection.o messages_types.o -o out-bm-encode-decode \
    -L/home/santa/fbthrift/thrift/lib/cpp2/.libs \
    -L/home/santa/fbthrift/thrift/lib/cpp/.libs \
    /home/santa/fbthrift/thrift/lib/cpp2/.libs/libthriftcpp2.a \
    /home/santa/fbthrift/thrift/lib/cpp/.libs/libthrift.a \
    /usr/local/lib/libfolly.a \
    -lglog \
    -ldouble-conversion \
    -lstdc++

echo gcc Echo_server.skeleton.o Echo.o echo_constants.o echo_reflection.o echo_types.o -o out-echo-server \
    -L/home/santa/fbthrift/thrift/lib/cpp2/.libs \
    -L/home/santa/fbthrift/thrift/lib/cpp/.libs \
    /home/santa/fbthrift/thrift/lib/cpp2/.libs/libthriftcpp2.a \
    /home/santa/fbthrift/thrift/lib/cpp/.libs/libthrift.a \
    /usr/local/lib/libfolly.a \
    -lglog \
    -ldouble-conversion \
    -lstdc++ \
    -lsnappy \
    -lpthread

