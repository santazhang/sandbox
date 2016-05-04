#!/bin/bash

DIR=$(dirname $0)
LD_DEBUG=files,statistics LD_LIBRARY_PATH=$DIR:$LD_LIBRARY_PATH $DIR/ld-linux-x86-64.so.2 \
    --inhibit-cache --inhibit-rpath $DIR/lua $DIR/lua
