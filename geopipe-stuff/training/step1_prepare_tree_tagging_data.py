#!/usr/bin/env python

from __future__ import print_function

import os
os.chdir(os.path.dirname(__file__))

from PIL import Image

from training_configurations import *
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
            os.rename(decoded_fn, decoded_fn2)
            decoded_fn = decoded_fn2
            im = Image.open(decoded_fn2)
            for x in range(0, im.width, MAX_TAGGING_IMAGE_WIDTH):
                for y in range(0, im.height, MAX_TAGGING_IMAGE_HEIGHT):
                    w = min(MAX_TAGGING_IMAGE_WIDTH, im.width - x)
                    h = min(MAX_TAGGING_IMAGE_HEIGHT, im.height - y)
                    im2 = im.crop((x, y, x + w, y + h))
                    crop_fn = os.path.join("03_tree_tagging", "%s.x%dy%d.tagging.png" % (main_fn, x, y))
                    im2.save(crop_fn)
                    print("  *** Saved %s" % crop_fn)

    elif e.endswith(".las"):
        for stddev_cap in [25,]:
            run_cmd("../build/las2img.bin -stddev_cap=%d '01_source_data/%s'" % (stddev_cap, e))
            run_cmd("convert '01_source_data/%s.las.decoded.z_stddev_gaussian.ppm' '02_decoded_data/%s.z_stddev_gaussian.cap%d.png'" % (main_fn, main_fn, stddev_cap))
            os.remove("01_source_data/%s.las.decoded.z_stddev_gaussian.ppm" % main_fn)
            os.remove("01_source_data/%s.las.decoded.z_stddev.ppm" % main_fn)
            os.remove("01_source_data/%s.las.decoded.z.ppm" % main_fn)
