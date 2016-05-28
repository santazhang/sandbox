#!/usr/bin/env python

import os
import sys

def my_cmd(cmd):
    print("+ %s" % cmd)
    os.system(cmd)

openjpeg2000_decoder_bin="/home/santa/Downloads/openjpeg-version.2.1/bin/opj_decompress"

if __name__ == "__main__":
    for fn in sys.argv[1:]:
        print(fn)
        bmp_fn = os.path.splitext(fn)[0] + ".RGB.bmp"
        my_cmd("%s -i '%s' -o '%s'" % (openjpeg2000_decoder_bin, fn, bmp_fn))

