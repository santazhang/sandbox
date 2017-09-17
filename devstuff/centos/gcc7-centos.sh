#!/bin/bash

OS_UNAME=`uname`

if [[ "$OS_UNAME" == "Linux" ]]; then
    N_CPU=`grep -c ^processor /proc/cpuinfo`
elif [[ "$OS_UNAME" == "Darwin" ]]; then
    N_CPU=`sysctl -n hw.ncpu`
else
    echo "  *** Unsupported platform: $OS_UNAME"
    exit 1
fi

set -x
set -e

cd $(dirname $0)

mkdir -p build-compiler-tmp
cd build-compiler-tmp

GCC_VER="7.2.0"

my_download() {
    local url=$1
    local fn=$2
    if [ ! -f $fn ]; then
        wget $url -O $fn.downloading
        mv $fn.downloading $fn
    fi
}

my_download https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VER}/gcc-${GCC_VER}.tar.gz gcc-${GCC_VER}.tar.gz
tar -xzf gcc-${GCC_VER}.tar.gz

cd gcc-${GCC_VER}
./contrib/download_prerequisites
# https://gist.github.com/jeetsukumaran/5224956
./configure \
    --enable-shared \
    --enable-threads=posix \
    --enable-__cxa_atexit \
    --enable-clocale=gnu \
    --disable-multilib \
    --enable-languages=c,c++

make -j$N_CPU
make install
