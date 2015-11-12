#!/bin/bash

set -e

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

GRPC_VERSION="05e466e35f759dd8f204d51ea33b082bd19ec037"

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
ROOT="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

cat <<ACTIVATE_SH > activate.sh
ROOT=$ROOT
export PATH=\$ROOT/bin:\$PATH
export DYLD_FALLBACK_LIBRARY_PATH=\$ROOT/lib:\$DYLD_FALLBACK_LIBRARY_PATH
export LD_LIBRARY_PATH=\$ROOT/lib:\$LD_LIBRARY_PATH
export LIBRARY_PATH=\$ROOT/lib:\$LIBRARY_PATH
export CPATH=\$ROOT/include:\$CPATH
export PYTHONPATH=\$ROOT/lib/python:\$PYTHONPATH
export LDFLAGS="-L\$ROOT/lib \$LDFLAGS"
export CPPFLAGS="-I\$ROOT/include \$CPPFLAGS"
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

get_grpc() {
    mkdir -p $ROOT/src
    pushd $ROOT/src > /dev/null
    if version_mismatch grpc/VERSION $GRPC_VERSION; then
        rm -rf grpc
        git clone https://github.com/grpc/grpc.git
        cd grpc
        git checkout $GRPC_VERSION
        git submodule update --init
        rm -rf .git
        make -j$N_CPU && make install prefix=$ROOT
        cd third_party/protobuf
        ./configure --prefix=$ROOT
        make && make install
        cd ../..
        make verify-install prefix=$ROOT
        echo $GRPC_VERSION > VERSION
    fi
    popd > /dev/null
}

get_grpc
