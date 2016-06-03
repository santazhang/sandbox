#!/usr/bin/env python

import os

from PIL import Image

def extract_tree_region(fn, out_fn, x, y, w, h):
    im = Image.open(fn)
    im2 = im.crop((x, y, x+w, y+h))
    im2.save(out_fn)

extract_tree_region(
    "/Volumes/Manhattan/research.sync/201203_LongIslandNY_0x6000m_4B/18TWL835090_201203_0x6000m_4B_1.RGB.bmp",
    "test-data/tree-valley-following-1.jpg",
    1540,
    1540,
    700,
    700
)

extract_tree_region(
    "/Volumes/Manhattan/research.sync/201203_LongIslandNY_0x6000m_4B/18TWL880180_201203_0x6000m_4B_1.RGB.bmp",
    "test-data/tree-valley-following-2.jpg",
    840,
    1440,
    700,
    700
)

extract_tree_region(
    "/Volumes/Manhattan/research.sync/201203_LongIslandNY_0x6000m_4B/18TWL880165_201203_0x6000m_4B_1.RGB.bmp",
    "test-data/tree-valley-following-3.jpg",
    1500,
    340,
    700,
    700
)
