#!/bin/bash

if [ -z "$WORKSPACE_GEOPIPE_STUFF" ]; then
    . $(dirname $0)/activate.sh
fi

DEPS_ROOT="$WORKSPACE_GEOPIPE_STUFF/deps"


mkdir -p $DEPS_ROOT/_install
mkdir -p $DEPS_ROOT/_build/openjpeg
cd $DEPS_ROOT/_build/openjpeg
cmake -DCMAKE_INSTALL_PREFIX:PATH=$DEPS_ROOT/_install ../../openjpeg
make
make install
