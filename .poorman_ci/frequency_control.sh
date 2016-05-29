#!/bin/bash

# Usage: frequency_control.sh project min_interval_h src_tree command

if [ "$#" -lt 3 ]; then
    echo "Usage: frequency_control.sh project min_interval_h src_tree command"
    exit 1
fi

project=$1
shift
min_interval=$1
shift
src_tree=$1
shift

SENTINEL_DIR=/tmp/poorman_ci_frequency_control/
mkdir -p $SENTINEL_DIR

OS_UNAME=`uname`
if [[ "$OS_UNAME" == 'Linux' ]]; then
    MD5_CMD="md5sum"
elif [[ "$OS_UNAME" == 'Darwin' ]]; then
    MD5_CMD="md5"
else
    echo "Unsupported OS: $OS_UNAME"
    exit 1
fi

project_md5=`echo "$project" | $MD5_CMD | cut -c1-16`
src_tree_md5=`find "$src_tree" -type f | sort | xargs -n 1 cat | \
              $MD5_CMD | cut -c1-16`
sentinel_fn="$SENTINEL_DIR/$project_md5-$src_tree_md5"

if find $SENTINEL_DIR -type f -mtime -$min_interval | \
        grep -q "$project_md5-$src_tree_md5"; then

    # Found file, so the min_interval has not passed yet.
    echo "  *** Skipping command '$@' ($min_interval hours interval)"
    exit 0

else
    rm -f $sentinel_fn
    touch $sentinel_fn
    echo "  *** Will run command '$@'"
    if $@; then
        touch $sentinel_fn
        echo "  *** Created sentinel file $sentinel_fn"
        echo "  *** Will not re-run until $min_interval hours passed"
    else
        rm -f $sentinel_fn
        echo "  *** Command '$@' failed"
        echo "  *** Removing sentinel file $sentinel_fn (if exists)"
    fi
fi
