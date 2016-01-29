#!/bin/bash

OS_UNAME=`uname`
DEVSTUFF_ROOT=${HOME}/Workspace/devstuff

COMMON_CXX_FLAGS="-std=c++0x -I ../gtest/include -I${DEVSTUFF_ROOT}/include"

if [[ "$OS_UNAME" == 'Linux' ]]; then
    g++ $COMMON_CXX_FLAGS -c proxygen_demo.cc -o proxygen_demo.o
    g++ proxygen_demo.o ${DEVSTUFF_ROOT}/lib/libfolly.a \
        ${DEVSTUFF_ROOT}/lib/libfollybenchmark.a \
        ${DEVSTUFF_ROOT}/lib/libwangle.a \
        ${DEVSTUFF_ROOT}/lib/libproxygenlib.a \
        ${DEVSTUFF_ROOT}/lib/libproxygenhttpserver.a \
        /usr/lib/x86_64-linux-gnu/libboost_system.a \
        /usr/lib/x86_64-linux-gnu/libboost_filesystem.a \
        /usr/lib/x86_64-linux-gnu/libboost_regex.a \
        /usr/lib/x86_64-linux-gnu/libboost_context.a \
        /usr/lib/x86_64-linux-gnu/libdouble-conversion.a \
        /usr/lib/x86_64-linux-gnu/libglog.a /usr/lib/x86_64-linux-gnu/libgflags.a \
        /usr/lib/x86_64-linux-gnu/libunwind.a /usr/lib/x86_64-linux-gnu/liblzma.a \
        /usr/lib/x86_64-linux-gnu/libevent.a \
        -lpthread -ldl -lz \
        -o proxygen_demo
    ldd -v proxygen_demo

elif [[ "$OS_UNAME" == 'Darwin' ]]; then
    g++ $COMMON_CXX_FLAGS -stdlib=libc++ -c proxygen_demo.cc -o proxygen_demo.o
    # if need to build with static gflags:
    # brew install gflags --with-static
    g++ ${DEVSTUFF_ROOT}/lib/libfolly.a \
        ${DEVSTUFF_ROOT}/lib/libfollybenchmark.a \
        ${DEVSTUFF_ROOT}/lib/libwangle.a \
        ${DEVSTUFF_ROOT}/lib/libproxygenlib.a \
        ${DEVSTUFF_ROOT}/lib/libproxygenhttpserver.a \
        /usr/local/lib/libevent.a \
        /usr/local/lib/libboost_context-mt.a \
        /usr/local/lib/libboost_system-mt.a \
        /usr/local/lib/libboost_filesystem-mt.a \
        /usr/local/lib/libboost_regex-mt.a \
        /usr/local/lib/libdouble-conversion.a \
        /usr/local/lib/libglog.a -lgflags \
        -lz -lcrypto -lssl -lproxygen_demo.o -o proxygen_demo
    otool -L proxygen_demo
fi
