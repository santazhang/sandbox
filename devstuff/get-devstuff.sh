#!/bin/bash

# set -e
# set -x

if ! grep -i "ubuntu 14.04" /etc/issue > /dev/null; then
    echo "  *** This script only supports Ubuntu 14.04 release."
    echo "  *** Please manually modify it to make things work."
    exit 1
fi

N_CPU=`grep -c ^processor /proc/cpuinfo`

FOLLY_VERSION="b57fd1bc00e1213b0dc4a8d597c9c7d0cf70402d"
WANGLE_VERSION="c1e434b725ca7e8336a0401f4bed548ea1aefc78"
PROXYGEN_VERSION="080b2b157915f7e970d9c406659ad4f8f9f0bedd"
FBTHRIFT_VERSION="181044fd78e0a26e77fb519e1cbd10238c2e32d6"
ROCKSDB_VERSION="f3170b6f6c88d2bb7992e2c4303d2c317af4cfa4"
PROTOBUF_VERSION="fbabf987e671aa37d11e1163e0175f99b32df5fe"
RE2_VERSION="08de04ce680858f52933d2554c656e56b891d9ce"

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

run_cmd() {
    echo + $@
    $@
}

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

get_gtest_1_7_0() {
    mkdir -p $ROOT/src
    pushd $ROOT/src > /dev/null
    if [ ! -d gtest-1.7.0 ]; then
        wget http://googletest.googlecode.com/files/gtest-1.7.0.zip
        unzip gtest-1.7.0.zip
        rm -f gtest-1.7.0.zip
    fi
    popd > /dev/null
}

get_gmock_1_7_0() {
    mkdir -p $ROOT/src
    pushd $ROOT/src > /dev/null
    if [ ! -d gmock-1.7.0 ]; then
        wget https://googlemock.googlecode.com/files/gmock-1.7.0.zip
        unzip gmock-1.7.0.zip
        rm -f gmock-1.7.0.zip
    fi
    popd > /dev/null
}

get_folly() {
    get_gtest_1_7_0
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
        ln -s $ROOT/src/gtest-1.7.0 gtest-1.7.0
        cd ..
        autoreconf -ivf
        CXXFLAGS="-pthread $CXXFLAGS" ./configure --prefix=$ROOT
        make -j$N_CPU && make install && cd .. && echo $FOLLY_VERSION > VERSION
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
        cd wangle
        cmake -DCMAKE_INSTALL_PREFIX=$ROOT .
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
        cd proxygen
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

get_folly
get_wangle
get_proxygen
get_fbthrift
get_rocksdb
get_protobuf
get_re2
