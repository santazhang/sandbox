#!/bin/bash

set -e
# set -x

OS_UNAME=`uname`

if [[ "$OS_UNAME" == "Linux" ]]; then
    if ! grep -i "ubuntu 14.04" /etc/issue > /dev/null; then
        echo "  *** This script only supports Ubuntu 14.04 release."
        echo "  *** Please manually modify it to make things work."
        exit 1
    fi
    N_CPU=`grep -c ^processor /proc/cpuinfo`
elif [[ "$OS_UNAME" == "Darwin" ]]; then
    MACOSX=1
    N_CPU=`sysctl -n hw.ncpu`
fi

FOLLY_VERSION="1dee433c3440fe58e046de3b2e59dfd7f1a5cb35"
WANGLE_VERSION="c1e434b725ca7e8336a0401f4bed548ea1aefc78"
PROXYGEN_VERSION="080b2b157915f7e970d9c406659ad4f8f9f0bedd"
FBTHRIFT_VERSION="181044fd78e0a26e77fb519e1cbd10238c2e32d6"
ROCKSDB_VERSION="2379944093530b04ff7dbd8f5d365a702b8dd1e6"
PROTOBUF_VERSION="49f24afb45b7add17af3ed4493fa0a94d1cc64da"
RE2_VERSION="d10ed8f2303f6c7303f5472736e2f535249b0a75"
GPERFTOOLS_VERSION="6627f9217d8897b297c6da038cfbcff6a3086cfa"

run_cmd() {
    echo + $@
    $@
}

if [ -n "$MACOSX" ]; then
    run_cmd brew install openssl boost boost-python double-conversion automake autoconf \
        libtool glog gflags libevent snappy autoconf-archive cmake
    run_cmd brew link --force openssl
else
    PKGS=(
        autoconf
        autoconf-archive
        bison
        cmake
        flex
        g++
        git
        gperf
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
fi

mkdir -p devstuff
cd devstuff

SOURCE=.
# Resolve $SOURCE until the file is no longer a symlink
while [ -h "$SOURCE" ]; do
    DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
    SOURCE="$(readlink "$SOURCE")"
    # If $SOURCE was a relative symlink, we need to resolve it relative to the
    # path where the symlink file was located
    [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
ROOT=$DIR

cat <<ACTIVATE_SH > activate.sh
ROOT=$ROOT
export PATH=\$ROOT/bin:\$PATH
export LD_LIBRARY_PATH=\$ROOT/lib:\$LD_LIBRARY_PATH
export LIBRARY_PATH=\$ROOT/lib:\$LIBRARY_PATH
export CPATH=\$ROOT/include:\$CPATH
export PYTHONPATH=\$ROOT/lib/python:\$PYTHONPATH
ACTIVATE_SH

. activate.sh

version_mismatch() {
    local file=$1
    local expected_ver=$2
    if [ -f $file ]; then
        local actual_ver=`cat $file`
        if [ "$actual_ver" == "$expected_ver" ]; then
            return 1
        fi
    fi
    return 0
}

get_gmock_1_7_0() {
    mkdir -p $ROOT/src
    pushd $ROOT/src > /dev/null
    if [ ! -f gmock-1.7.0.zip ]; then
        wget https://googlemock.googlecode.com/files/gmock-1.7.0.zip
    fi
    if [ ! -d gmock-1.7.0 ]; then
        unzip gmock-1.7.0.zip
    fi
    popd > /dev/null
}

get_macosx_hack() {
    mkdir -p $ROOT/src
    mkdir -p $ROOT/lib
    mkdir -p $ROOT/include
    pushd $ROOT/src > /dev/null
    rm -rf macosx_hack
    git clone https://gist.github.com/256985a658d765abed93.git macosx_hack
    cd macosx_hack
    make
    cp pthread_macosx_hack.h numa.h numacompat1.h $ROOT/include
    cp libnuma.a $ROOT/lib
    popd > /dev/null
}

get_folly() {
    get_gmock_1_7_0
    mkdir -p $ROOT/src
    pushd $ROOT/src > /dev/null
    if version_mismatch folly/VERSION $FOLLY_VERSION; then
        rm -rf folly
        git clone https://github.com/facebook/folly.git
        cd folly
        git checkout $FOLLY_VERSION
        rm -rf .git
        cd folly/test
        rm -f gtest-1.7.0
        ln -s $ROOT/src/gmock-1.7.0/gtest gtest-1.7.0
        cd ..
        autoreconf -ivf

        if [ -n "$MACOSX" ]; then
            # Due to MACOSX filesystem being case-insensitive, gcc will include
            # String.h on `#include <string.h>`. To avoid this wrong behavior,
            # do out-of-source build.
            cd $ROOT/src/folly
            ./folly/configure --prefix=$ROOT
            # mkdir -p test/gtest-1.7.0
            # cd test/gtest-1.7.0
            # ln -s ../../folly/test/gtest-1.7.0/include .
            # mv src src.orig
            # ln -s ../../folly/test/gtest-1.7.0/src .
            # cd ../..
            mkdir -p build
            cd build
            ln -s ../folly/build/generate_escape_tables.py .
            ln -s ../folly/build/generate_format_tables.py .
            ln -s ../folly/build/generate_varint_tables.py .
            cd ../folly
            ln -s ../folly-config.h .
            cd ..
        else
            CXXFLAGS="-pthread $CXXFLAGS" ./configure --prefix=$ROOT
        fi

        make -j$N_CPU && make install && cd $ROOT/src/folly && echo $FOLLY_VERSION > VERSION
    fi
    popd > /dev/null
}

get_wangle() {
    get_gmock_1_7_0
    mkdir -p $ROOT/src
    pushd $ROOT/src > /dev/null
    if version_mismatch wangle/VERSION $WANGLE_VERSION; then
        rm -rf wangle
        git clone https://github.com/facebook/wangle.git
        cd wangle
        git checkout $WANGLE_VERSION
        rm -rf .git

        if [ -n "$MACOSX" ]; then
            cat $ROOT/src/macosx_hack/wangle-on-macosx.patch
            patch -p1 < $ROOT/src/macosx_hack/wangle-on-macosx.patch
        fi

        cd wangle
        mkdir -p gmock/src/gmock-stamp
        cd gmock/src
        ln -s $ROOT/src/gmock-1.7.0.zip gmock-1.7.0.zip
        cd ../..
        cmake -DCMAKE_INSTALL_PREFIX=$ROOT .
        cat /dev/null > gmock/src/gmock-stamp/download-gmock.cmake
        make -j$N_CPU && make install && cd .. && echo $WANGLE_VERSION > VERSION
    fi
    popd > /dev/null
}

get_proxygen() {
    get_gmock_1_7_0
    mkdir -p $ROOT/src
    pushd $ROOT/src > /dev/null
    if version_mismatch proxygen/VERSION $PROXYGEN_VERSION; then
        rm -rf proxygen
        git clone https://github.com/facebook/proxygen.git
        cd proxygen
        git checkout $PROXYGEN_VERSION
        rm -rf .git
        if [ -n "$MACOSX" ]; then
            cat $ROOT/src/macosx_hack/proxygen-on-macosx.patch
            patch -p1 < $ROOT/src/macosx_hack/proxygen-on-macosx.patch
            mv proxygen/lib/utils/Time.h proxygen/lib/utils/TimeUtils.h
            grep -r -l "#include.*/Time\.h" proxygen/ | xargs -n 1 sed -i "" "s/\/Time\.h/\/TimeUtils\.h/g"
            sed -i "" "s/	Time\.h \\\\/	TimeUtils\.h \\\\/g" proxygen/lib/utils/Makefile.am
            touch proxygen/lib/ssl/dummy.cpp
            sed -i "" "s/libproxygenssl_la_SOURCES =/libproxygenssl_la_SOURCES = dummy\.cpp/g" proxygen/lib/ssl/Makefile.am
            sed -i "" "s/-lboost_thread/-lboost_thread-mt/g" proxygen/configure.ac
        fi
        cd proxygen
        mkdir -p lib/test
        cd lib/test
        ln -s $ROOT/src/gmock-1.7.0 gmock-1.7.0
        cd ../..
        autoreconf -if
        ./configure --prefix=$ROOT
        make -j$N_CPU && make install && \
            cd .. && echo $PROXYGEN_VERSION > VERSION
    fi
    popd > /dev/null
}

get_fbthrift() {
    mkdir -p $ROOT/src
    pushd $ROOT/src > /dev/null
    if version_mismatch fbthrift/VERSION $FBTHRIFT_VERSION; then
        rm -rf fbthrift
        git clone https://github.com/facebook/fbthrift.git
        cd fbthrift
        git checkout $FBTHRIFT_VERSION
        rm -rf .git
        if [ -n "$MACOSX" ]; then
            cat $ROOT/src/macosx_hack/fbthrift-on-macosx.patch
            patch -p1 < $ROOT/src/macosx_hack/fbthrift-on-macosx.patch
            sed -i "" "s/<pthread\.h>/<pthread_macosx_hack\.h>/g" thrift/lib/cpp/concurrency/Mutex-impl.h
            for file in thrift/configure.ac thrift/lib/cpp/Makefile.am thrift/lib/cpp2/Makefile.am thrift/lib/cpp2/test/Makefile.am; do
                sed -i "" "s/-lboost_thread/-lboost_thread-mt/g" $file
            done
        fi
        cd thrift
        autoreconf -if
        ./configure --prefix=$ROOT --without-php
        make -j$N_CPU && \
            make install PY_INSTALL_HOME=$ROOT \
                         PY_INSTALL_ARGS="--home=$ROOT" \
                         PY_RUN_ENV="PYTHONPATH=$ROOT/lib/python" && \
            cd .. && echo $FBTHRIFT_VERSION > VERSION
    fi
    popd > /dev/null
}

get_rocksdb() {
    mkdir -p $ROOT/src
    pushd $ROOT/src > /dev/null
    if version_mismatch rocksdb/VERSION $ROCKSDB_VERSION; then
        rm -rf rocksdb
        git clone https://github.com/facebook/rocksdb.git
        cd rocksdb
        git checkout $ROCKSDB_VERSION
        rm -rf .git
        make -j$N_CPU && INSTALL_PATH=$ROOT make install && \
            echo $ROCKSDB_VERSION > VERSION
    fi
    popd > /dev/null
}

get_protobuf() {
    get_gmock_1_7_0
    mkdir -p $ROOT/src
    pushd $ROOT/src > /dev/null
    if version_mismatch protobuf/VERSION $PROTOBUF_VERSION; then
        rm -rf protobuf
        git clone https://github.com/google/protobuf.git
        cd protobuf
        git checkout $PROTOBUF_VERSION
        rm -rf .git
        ln -s $ROOT/src/gmock-1.7.0 gmock
        ./autogen.sh
        ./configure --prefix=$ROOT
        make -j$N_CPU && make install && echo $PROTOBUF_VERSION > VERSION
    fi
    popd > /dev/null
}

get_re2() {
    mkdir -p $ROOT/src
    pushd $ROOT/src > /dev/null
    if version_mismatch re2/VERSION $RE2_VERSION; then
        rm -rf re2
        git clone https://github.com/google/re2.git
        cd re2
        git checkout $RE2_VERSION
        rm -rf .git
        make -j$N_CPU && make install prefix=$ROOT && echo $RE2_VERSION > VERSION
    fi
    popd > /dev/null
}

get_gperftools() {
    mkdir -p $ROOT/src
    pushd $ROOT/src > /dev/null
    if version_mismatch gperftools/VERSION $GPERFTOOLS_VERSION; then
        rm -rf gperftools
        git clone https://github.com/gperftools/gperftools.git
        cd gperftools
        git checkout $GPERFTOOLS_VERSION
        rm -rf .git
        ./autogen.sh
        ./configure --prefix=$ROOT
        make -j$N_CPU && make install && echo $GPERFTOOLS_VERSION > VERSION
    fi
    popd > /dev/null
}

if [ -n "$MACOSX" ]; then
    get_macosx_hack
fi

get_folly
get_wangle
get_proxygen
get_fbthrift
get_rocksdb
get_protobuf
get_re2
get_gperftools
