#!/bin/bash

(
    cd yajl-2.1.0
    ./configure
    make
)

(
    cd jansson-2.7
    mkdir -p build
    cd build
    cmake ..
    make
)

(
    cd jsoncpp-7bd75b0
    mkdir -p build
    cd build
    cmake ..
    make
)

