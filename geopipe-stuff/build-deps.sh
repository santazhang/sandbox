#!/bin/bash

if [ -z "$WORKSPACE_GEOPIPE_STUFF" ]; then
    . $(dirname $0)/activate.sh
fi

OS_UNAME=`uname`

if [[ "$OS_UNAME" == "Linux" ]]; then
    N_CPU=`grep -c ^processor /proc/cpuinfo`
elif [[ "$OS_UNAME" == "Darwin" ]]; then
    N_CPU=`sysctl -n hw.ncpu`
else
    echo "  *** Unsupported platform: $OS_UNAME"
    exit 1
fi

DEPS_ROOT="$WORKSPACE_GEOPIPE_STUFF/deps"

mkdir -p $DEPS_ROOT/_install
mkdir -p $DEPS_ROOT/_build/openjpeg
cd $DEPS_ROOT/_build/openjpeg
cmake -DCMAKE_INSTALL_PREFIX:PATH=$DEPS_ROOT/_install ../../openjpeg
make -j$N_CPU
make install

cd $DEPS_ROOT/_build
rsync -avx ../LAStools .
cd LAStools
make -j$N_CPU
cd LASlib
cp lib/liblas.a $DEPS_ROOT/_install/lib
rsync -avx inc/ $DEPS_ROOT/_install/include/laslib/
cd ../LASzip
cp src/*.hpp $DEPS_ROOT/_install/include/laslib/

cd $DEPS_ROOT/_build
rsync -avx ../caffe .
cd caffe
cp ../../caffe-Makefile.config Makefile.config
make -j$N_CPU
