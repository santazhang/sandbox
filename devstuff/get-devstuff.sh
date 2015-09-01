#!/bin/bash

set -e
set -x

sudo apt-get install -yq \
    autoconf-archive \
    libcap-dev

mkdir devstuff
cd devstuff

git clone https://github.com/facebook/fbthrift.git
git clone https://github.com/facebook/wangle.git
git clone https://github.com/facebook/proxygen.git
git clone https://github.com/facebook/rocksdb.git

(
    cd fbthrift/thrift
    ./build/deps_ubuntu_14.04.sh
    cd ../..
    ln -s fbthrift/thrift/build/deps/folly folly
    cd proxygen
    ln -s ../fbthrift/thrift/build/deps/folly folly
)

(
    cd wangle/wangle
    cmake .
    make
    sudo make install
)

(
    cd fbthrift/thrift
    autoreconf -if
    ./configure
    make
)

(
    cd proxygen/proxygen
    autoreconf -ivf
    ./configure
    make
    make check
)

(
    cd rocksdb
    make
)
