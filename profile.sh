#!/bin/bash

SOURCE="${BASH_SOURCE[0]}"
# Resolve $SOURCE until the file is no longer a symlink
while [ -h "$SOURCE" ]; do
    DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
    SOURCE="$(readlink "$SOURCE")"
    # If $SOURCE was a relative symlink, we need to resolve it relative to the
    # path where the symlink file was located
    [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

DEVSTUFF_ROOT=$DIR/../devstuff

source $DEVSTUFF_ROOT/activate.sh

target=`which $1`
appname=`basename $1`
GPERFTOOLS_REPORT=`mktemp -q /tmp/gperftools-profile-${appname}.XXXXXX` || exit 1

shift

set -x

env LD_PRELOAD=$DEVSTUFF_ROOT/libprofiler.so \
    DYLD_INSERT_LIBRARIES=$DEVSTUFF_ROOT/libprofiler.dylib \
    CPUPROFILE=$GPERFTOOLS_REPORT $target $@

pprof --text $target $GPERFTOOLS_REPORT
rm -f $GPERFTOOLS_REPORT
