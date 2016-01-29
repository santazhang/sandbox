#!/bin/bash

OS_UNAME=`uname`
DEVSTUFF_ROOT=${HOME}/Workspace/devstuff

if [[ "$OS_UNAME" == 'Linux' ]]; then
    g++ -std=c++0x -I${DEVSTUFF_ROOT}/include -c folly_demo.cc -o folly_demo.o
	g++ folly_demo.o ${DEVSTUFF_ROOT}/lib/libfolly.a /usr/lib/x86_64-linux-gnu/libboost_system.a \
		/usr/lib/x86_64-linux-gnu/libboost_filesystem.a \
		/usr/lib/x86_64-linux-gnu/libboost_context.a \
		/usr/lib/x86_64-linux-gnu/libdouble-conversion.a \
		/usr/lib/x86_64-linux-gnu/libglog.a /usr/lib/x86_64-linux-gnu/libgflags.a \
		/usr/lib/x86_64-linux-gnu/libunwind.a /usr/lib/x86_64-linux-gnu/liblzma.a \
		/usr/lib/x86_64-linux-gnu/libevent.a \
		-lpthread -ldl \
		-o folly_demo
	ldd -v folly_demo

elif [[ "$OS_UNAME" == 'Darwin' ]]; then
    g++ -std=c++0x -stdlib=libc++ -I${DEVSTUFF_ROOT}/include -c folly_demo.cc -o folly_demo.o
	# if need to build with static gflags:
	# brew install gflags --with-static
	g++ ${DEVSTUFF_ROOT}/lib/libfolly.a \
        /usr/local/lib/libevent.a \
        /usr/local/lib/libboost_context-mt.a \
        /usr/local/lib/libboost_system-mt.a \
        /usr/local/lib/libboost_filesystem-mt.a \
        /usr/local/lib/libdouble-conversion.a \
        /usr/local/lib/libglog.a -lgflags folly_demo.o -o folly_demo
	otool -L folly_demo
fi
