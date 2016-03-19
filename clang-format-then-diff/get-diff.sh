#!/bin/bash

export PATH=$HOME/Workspace/clang-stuff-dist/bin/:$PATH

set -x

cd $(dirname $0)

rm -rf sparsehash
rm -rf sparsehash-c11

cp -r $HOME/Workspace/watched/sparsehash/src/sparsehash sparsehash
cp -r $HOME/Workspace/watched/sparsehash-c11/sparsehash sparsehash-c11

find sparsehash sparsehash-c11 -type f -exec clang-format -i -style=file {} \;

diff -ru sparsehash sparsehash-c11 > diff.diff

rm -rf sparsehash
rm -rf sparsehash-c11
