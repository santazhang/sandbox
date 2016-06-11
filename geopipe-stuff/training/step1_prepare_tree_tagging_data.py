#!/usr/bin/env python

from __future__ import print_function

import os
os.chdir(os.path.dirname(__file__))

import shutil
from utils import *

for e in os.listdir("01_source_data"):
    if e.startswith(".") or (not e.endswith(".jp2") and not e.endswith(".las")):
        continue

    main_fn = e.split(".")[0]

    if e.endswith(".jp2"):
        run_cmd("'../scripts/decode_4band_jp2.sh' '01_source_data/%s'" % e)
        for ext in [".decoded.CMYK-Y.png",
                    ".decoded.HSB-S.png",
                    ".decoded.LAB-A.png",
                    ".decoded.RGB.png",
                    ".decoded.IR.png"]:
            decoded_fn = os.path.join("01_source_data", main_fn + ext)
            if not os.path.exists(decoded_fn) or ext != ".decoded.RGB.png":
                continue
            decoded_fn2 = os.path.join("02_decoded_data", main_fn + ext)
            decoded_fn3 = os.path.join("03_tree_tagging", main_fn + ext)
            shutil.copyfile(decoded_fn, decoded_fn3)
            os.rename(decoded_fn, decoded_fn2)

    elif e.endswith(".las"):
        for stddev_cap in [25,]:
            run_cmd("../build/las2img.bin -stddev_cap=%d '01_source_data/%s'" % (stddev_cap, e))
            run_cmd("convert '01_source_data/%s.las.decoded.z_stddev_gaussian.ppm' '02_decoded_data/%s.z_stddev_gaussian.cap%d.png'" % (main_fn, main_fn, stddev_cap))
            os.remove("01_source_data/%s.las.decoded.z_stddev_gaussian.ppm" % main_fn)
            os.remove("01_source_data/%s.las.decoded.z_stddev.ppm" % main_fn)
            os.remove("01_source_data/%s.las.decoded.z.ppm" % main_fn)
