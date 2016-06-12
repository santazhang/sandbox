#!/usr/bin/env python

from __future__ import print_function

import os
os.chdir(os.path.dirname(__file__))

import re
from PIL import Image, ImageDraw
from training_configurations import *
from utils import *


# rgb, cmyk_y, hsb_s, ir, lab_a
# or None
def get_image_channels(fpath):
    fn = os.path.basename(fpath)
    sp = fn.split(".")
    if "RGB" in sp:
        return "rgb"
    elif "CMYK-Y" in sp:
        return "cmyk_y"
    elif "HSB-S" in sp:
        return "hsb_s"
    elif "IR" in sp:
        return "ir"
    elif "LAB-A" in sp:
        return "lab_a"


uniq_token = random_token(10)

mark_files = set()
image_files = {}  # main fn -> full path
image_ext = [".png", ".bmp", ".jpg"]

for d in ["01_source_data", "02_decoded_data", "03_tree_tagging"]:
    for e in os.listdir(d):
        epath = os.path.join(d, e)
        if e.startswith("."):
            continue
        if not re.match("[0-9]{2}[A-Z]{3}[0-9]+.*", e):
            print("SKIP: %s" % epath)
            continue
        if e.endswith(".mark.txt"):
            mark_files.add(epath)
        else:
            if get_image_channels(e) is None:
                print("SKIP: %s" % epath)
                continue
            for ext in image_ext:
                if e.endswith(ext):
                    image_files[e] = epath
                    break

# eg: "18TWL835090" -> full path of 18TWL835090.mark.txt
mark_files_map = {}

print()
print("* Mark files:")
for fn in mark_files:
    print(fn)
    main_fn = os.path.basename(fn).split(".")[0]
    mark_files_map[main_fn] = fn
print()

print("* Image files:")
for k in image_files:
    print(image_files[k])
print()

print("Start preparing training data.")
for k in image_files:
    image_fn = image_files[k]
    channels = get_image_channels(image_fn)

    geo_id = os.path.basename(image_fn)[0:11]
    if geo_id not in mark_files_map:
        print("  *** Cannot find mark file for: %s" % image_fn)
        continue
    mark_fn = mark_files_map[geo_id]
    print("Processing: %s" % image_fn)
    print("Image mark found: %s" % mark_fn)
    im = Image.open(image_fn)
    print("Image size: %d x %d" % (im.width, im.height))
    im2 = Image.new("L", im.size, 0)  # gray image (256 levels as tile type, 0 unused)
    draw = ImageDraw.Draw(im2)
    with open(mark_fn) as f:
        while True:
            l = f.readline()
            if l == "":
                break
            l = l.strip()
            sp = l.split()
            tile_type = int(sp[0]) # * 100
            if tile_type <= 0:
                continue
            xy = []
            for i in range(1, len(sp), 2):
                xy += (float(sp[i]) * im.width, float(sp[i+1]) * im.height),
            draw.polygon(xy, fill=tile_type)
    # im2.show()

    step_x = TILE_STEP_X
    step_y = TILE_STEP_Y
    for tile_border_len_a in TILE_SIZES:
        tile_width = tile_border_len_a
        tile_height = tile_border_len_a
        print("Using tile size: %dx%d" % (tile_width, tile_height))

        tree_tile_dir = "04_training_data_preview/tree_%dx%d_%s" % (tile_width, tile_height, channels)
        not_tree_tile_dir = "04_training_data_preview/not_tree_%dx%d_%s" % (tile_width, tile_height, channels)
        for d in [tree_tile_dir, not_tree_tile_dir]:
            mkdir_p(d)

        pct = -1
        for top_y in range(0, im.height - step_y, step_y):
            new_pct = 100 * top_y / im.height
            if new_pct != pct:
                print("Done %d%%" % new_pct)
                pct = new_pct
            for left_x in range(0, im.width - step_x, step_x):
                im2_crop = im2.crop((left_x, top_y, left_x + tile_width, top_y + tile_height))
                tree_cnt = 0
                not_tree_cnt = 0
                unknown_cnt = 0
                for y in range(tile_height):
                    for x in range(tile_width):
                        pxl = im2_crop.getpixel((x, y))
                        if pxl == TILE_TYPE_NOT_TREE:
                            not_tree_cnt += 1
                        elif pxl == TILE_TYPE_TREE:
                            tree_cnt += 1
                        else:
                            unknown_cnt += 1
                tree_ratio = float(tree_cnt) / tile_width / tile_height
                not_tree_ratio = float(not_tree_cnt) / tile_width / tile_height
                if tree_ratio > 0.6 or not_tree_ratio > 0.6:
                    im_crop = im.crop((left_x, top_y, left_x + tile_width, top_y + tile_height))
                    if tree_ratio > 0.6:
                        extract_fn = os.path.join(tree_tile_dir, "tmp-%s.png" % uniq_token)
                        im_crop.save(extract_fn)
                        new_fn = "%s_%02d_%04d_%04d.png" % (geo_id, tile_border_len_a, left_x, top_y)
                        new_dir = md5_of_str(new_fn)[0:2]
                        mkdir_p(os.path.join(tree_tile_dir, new_dir))
                        os.rename(extract_fn, os.path.join(tree_tile_dir, new_dir, new_fn))
                    elif not_tree_ratio > 0.6:
                        extract_fn = os.path.join(not_tree_tile_dir, "tmp-%s.png" % uniq_token)
                        im_crop.save(extract_fn)
                        new_fn = "%s_%02d_%04d_%04d.png" % (geo_id, tile_border_len_a, left_x, top_y)
                        new_dir = md5_of_str(new_fn)[0:2]
                        mkdir_p(os.path.join(not_tree_tile_dir, new_dir))
                        os.rename(extract_fn, os.path.join(not_tree_tile_dir, new_dir, new_fn))
    print()
