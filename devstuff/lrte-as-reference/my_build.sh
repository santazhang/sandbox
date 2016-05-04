#!/bin/bash

# santa: doing this on b20

cd $(dirname $0)

git clean -xdf .

export TAR_DIR=upstream
export JFLAGS="-j40"

./grte/download.sh
./grte/prepare-sources.sh

runtime_root=/scratch2/santa/my-grte
mkdir -p $runtime_root

./grte/grte-build $runtime_root ./build-tmp
./grte/grte-package $runtime_root ./build-tmp
