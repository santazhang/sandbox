#!/bin/bash

# Build libclang.dylib for code analysis on MACOSX
#
# Yang Zhang <y@yzhang.net>, 2016
# See LICENSE file for copyright notice

OS_UNAME=`uname`
if [[ "$OS_UNAME" == "Darwin" ]]; then
    N_CPU=`sysctl -n hw.ncpu`
    SO_EXT="dylib"
elif [[ "$OS_UNAME" == "Linux" ]]; then
    N_CPU=`grep -c ^processor /proc/cpuinfo`
    SO_EXT="so"
else
    echo "  *** Unsupported platform: $OS_UNAME"
    exit 1
fi

set -x
set -e

OUT_DIR=$(pwd)/clang-stuff
rm -rf $OUT_DIR
mkdir -p $OUT_DIR
cd $OUT_DIR
mkdir -p src dist/bin dist/lib dist/pylib

# https://gist.github.com/larsch/6823355
CLANG_VER="5.0.0"
# IWYU_VER="0.7"
cd $OUT_DIR/src
wget http://llvm.org/releases/$CLANG_VER/llvm-$CLANG_VER.src.tar.xz
wget http://llvm.org/releases/$CLANG_VER/cfe-$CLANG_VER.src.tar.xz
wget http://llvm.org/releases/$CLANG_VER/clang-tools-extra-$CLANG_VER.src.tar.xz
# wget http://include-what-you-use.org/downloads/include-what-you-use-$IWYU_VER.src.tar.gz
mkdir -p llvm/tools/clang/tools/extra
tar xfJ llvm-$CLANG_VER.src.tar.xz -C llvm --strip-components=1
tar xfJ cfe-$CLANG_VER.src.tar.xz -C llvm/tools/clang --strip-components=1
tar xfJ clang-tools-extra-$CLANG_VER.src.tar.xz -C llvm/tools/clang/tools/extra --strip-components=1
# tar xfz include-what-you-use-$IWYU_VER.src.tar.gz

cd $OUT_DIR/src
cd llvm
# echo "add_clang_subdirectory(include-what-you-use)" >> tools/clang/tools/CMakeLists.txt
mkdir -p build
cd build
cmake -DCMAKE_INSTALL_PREFIX=$OUT_DIR ..
make -j$N_CPU
make install
exit 0
# cp -r ../tools/clang/bindings/python/clang $OUT_DIR/dist/pylib
#
# cd $OUT_DIR
# cp bin/clang-format dist/bin
# cp bin/clang-tidy dist/bin
# cp bin/include-what-you-use dist/bin
# cp lib/libclang.$SO_EXT dist/lib
# cp -r lib/clang dist/lib/
