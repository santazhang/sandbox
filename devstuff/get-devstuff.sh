#!/bin/bash

# set -e
# set -x

PKGS=(
    autoconf
    autoconf-archive
    bison
    flex
    g++
    git
    libboost-all-dev
    libcap-dev
    libdouble-conversion-dev
    libevent-dev
    libgoogle-glog-dev
    libkrb5-dev
    libnuma-dev
    libsasl2-dev
    libsnappy-dev
    libssl-dev
    libtool
    make
    scons
    zip
)

run_cmd() {
    echo + $@
    $@
}

if ! grep -i "ubuntu 14.04" /etc/issue > /dev/null; then
    echo "  *** This script only supports Ubuntu 14.04 release."
    echo "  *** Please manually modify it to make things work."
    exit 1
fi

PKGS_TO_INSTALL=""

for pkg in "${PKGS[@]}"
do
    if ! dpkg -s $pkg > /dev/null 2>&1; then
        PKGS_TO_INSTALL="$PKGS_TO_INSTALL $pkg"
    fi
done

if [ ! -z "$PKGS_TO_INSTALL" ]; then
    run_cmd sudo apt-get install $PKGS_TO_INSTALL
fi

mkdir -p devstuff
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
