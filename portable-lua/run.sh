#!/bin/bash

DIR=$(dirname $0)
LD_LIBRARY_PATH=$DIR:$LD_LIBRARY_PATH $DIR/ld-linux-x86-64.so.2 $DIR/lua
