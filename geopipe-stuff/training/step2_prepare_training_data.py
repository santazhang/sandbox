#!/usr/bin/env python

from __future__ import print_function

import os
os.chdir(os.path.dirname(__file__))

import re
from PIL import Image

mark_files = set()
image_files = set()
image_ext = [".png", ".bmp", ".jpg"]

for d in ["01_source_data", "02_decoded_data", "03_tree_tagging"]:
    for e in os.listdir(d):
        epath = os.path.abspath(e)
        if e.startswith("."):
            continue
        if not re.match("[0-9]{2}[A-Z]{3}[0-9]+.*", e):
            print("SKIP: %s" % epath)
            continue
        if e.endswith(".mark.txt"):
            mark_files.add(epath)
        else:
            for ext in image_ext:
                if e.endswith(ext):
                    image_files.add(epath)
                    break

print("* Mark files:")
for fn in mark_files:
    print(fn)
print()

print("* Image files:")
for fn in image_files:
    print(fn)
print()
