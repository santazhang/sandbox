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
        if "0x1500m" not in e:
            print("  *** Currently only supports 0.15 resolution images.")
            print("  *** SKIPPING: %s" % e)
            continue

        run_cmd("'../scripts/decode_4band_jp2.sh' '01_source_data/%s'" % e)
        for ext in [#".decoded.CMYK-Y.png",
                    #".decoded.HSB-S.png",
                    #".decoded.LAB-A.png",
                    ".decoded.RGB.png",
                    ".decoded.IR.png"]:
            decoded_fn = os.path.join("01_source_data", main_fn + ext)
            decoded_fn2 = os.path.join("02_decoded_data", main_fn + ext)
            if not os.path.exists(decoded_fn):
                continue
            elif ext == ".decoded.RGB.png":
                decoded_fn3 = os.path.join("03_tree_tagging", main_fn + ext)
                shutil.copyfile(decoded_fn, decoded_fn3)
            os.rename(decoded_fn, decoded_fn2)

    elif e.endswith(".las"):
        run_cmd("../scripts/run_activated.sh ../build/las2stats -resolution=0.15 '01_source_data/%s' '02_decoded_data/%s.0x1500m.z_hint'" % (e, e))
