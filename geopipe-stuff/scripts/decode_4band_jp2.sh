#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 jp2-file"
    exit 1
fi

which convert > /dev/null 2>&1 || { echo "  *** ImageMagick not installed!" ; exit 1; }

if [ -z "$WORKSPACE_GEOPIPE_STUFF" ]; then
    . $(dirname $0)/../activate.sh
fi

export LD_LIBRARY_PATH=$WORKSPACE_GEOPIPE_STUFF/deps/_install/lib:$LD_LIBRARY_PATH

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
    -channel RGB -alpha off "$TMP_DIR/$IMG_BASENAME.decoded.RGB.png"
mv "$TMP_DIR/$IMG_BASENAME.decoded.RGB.png" "$IMG_DIR"

convert "$IMG_DIR/$IMG_BASENAME.decoded.RGB.png" \
    -colorspace LAB -separate "$TMP_DIR/$IMG_BASENAME.decoded.LAB-%d.png"
mv "$TMP_DIR/$IMG_BASENAME.decoded.LAB-1.png" \
    "$IMG_DIR/$IMG_BASENAME.decoded.LAB-A.png"

convert "$IMG_DIR/$IMG_BASENAME.decoded.RGB.png" \
    -colorspace HSB -separate "$TMP_DIR/$IMG_BASENAME.decoded.HSB-%d.png"
mv "$TMP_DIR/$IMG_BASENAME.decoded.HSB-1.png" \
    "$IMG_DIR/$IMG_BASENAME.decoded.HSB-S.png"

convert "$IMG_DIR/$IMG_BASENAME.decoded.RGB.png" \
    -colorspace CMYK -separate "$TMP_DIR/$IMG_BASENAME.decoded.CMYK-%d.png"
mv "$TMP_DIR/$IMG_BASENAME.decoded.CMYK-2.png" \
    "$IMG_DIR/$IMG_BASENAME.decoded.CMYK-Y.png"

convert "$TMP_DIR/$IMG_BASENAME.tmp-opj_decompress.png" \
    -channel RGBA -separate "$TMP_DIR/$IMG_BASENAME.tmp-extract-band-%d.png"
mv "$TMP_DIR/$IMG_BASENAME.tmp-extract-band-3.png" \
    "$IMG_DIR/$IMG_BASENAME.decoded.IR.png"

rm -rf "$TMP_DIR"
