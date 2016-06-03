#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 jp2-file"
    exit 1
fi

which convert > /dev/null 2>&1 || { echo "  *** ImageMagick not installed!" ; exit 1; }

if [ -z "$WORKSPACE_GEOPIPE_STUFF" ]; then
    . $(dirname $0)/../activate.sh
fi

opj_decompress_bin=$WORKSPACE_GEOPIPE_STUFF/deps/_install/bin/opj_decompress

if [ ! -f $opj_decompress_bin ]; then
    echo "  *** opj_decompress not compiled"
    echo "  *** Run this: $WORKSPACE_GEOPIPE_STUFF/build-deps.sh"
    exit 1
fi

TMP_DIR=$(mktemp -dt "$(basename $0)".XXXXXX)
IMG_DIR=$(dirname $1)
IMG_BASENAME=$(basename $1 .jp2)

set -x

"$opj_decompress_bin" \
    -i "$1" \
    -o "$TMP_DIR/$IMG_BASENAME.tmp-opj_decompress.png"

convert "$TMP_DIR/$IMG_BASENAME.tmp-opj_decompress.png" \
    -channel RGB -alpha off "$IMG_DIR/$IMG_BASENAME.RGB.jpg"

convert "$TMP_DIR/$IMG_BASENAME.tmp-opj_decompress.png" \
    -channel RGBA -separate "$TMP_DIR/$IMG_BASENAME.tmp-extract-band-%d.jpg"

mv "$TMP_DIR/$IMG_BASENAME.tmp-extract-band-3.jpg" \
    "$IMG_DIR/$IMG_BASENAME.IR.jpg"

rm -rf "$TMP_DIR"
