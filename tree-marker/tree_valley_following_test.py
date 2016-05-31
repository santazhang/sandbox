#!/usr/bin/env python

import os
from PIL import Image, ImageDraw

#init_fill_color=(255,255,255)
init_fill_color=(0,0,0)

def to_gray(rgb_tuple):
    return (rgb_tuple[0] + rgb_tuple[1] + rgb_tuple[2]) / 3.0

def phase_i_init(im, fn_base):
    print(im.getpixel((0, 0)))
    threshold_lst = []
    for y in range(im.height):
        for x in range(im.width):
            pxl = im.getpixel((x, y))
            if pxl != init_fill_color:
                threshold_lst += to_gray(pxl),
    threshold_lst.sort()
    threshold_cut_percent = 0.15
    threshold_cut_hi_percent = 1 - 0.2
    threshold = threshold_lst[int(len(threshold_lst) * threshold_cut_percent)]
    threshold_hi = threshold_lst[int(len(threshold_lst) * threshold_cut_hi_percent)]
    print("threshold=%f" % threshold)
    im2 = im.copy()
    for y in range(im2.height):
        for x in range(im2.width):
            pxl = im2.getpixel((x, y))
            g = to_gray(pxl)
            if g < threshold or g > threshold_hi:
                im2.putpixel((x, y), (0,0,0))
    im2.save(fn_base + ".threshold_cut.bmp")

    im3 = im2.copy()
    
    # find local minima 3x3 sliding window
    local_minima = set()
    marked_pxls = set()
    for y in range(1, im.height - 1):
        for x in range(1, im.width - 1):
            nei = [
                to_gray(im2.getpixel((x-1,y-1))),
                to_gray(im2.getpixel((x,y-1))),
                to_gray(im2.getpixel((x+1,y-1))),
                to_gray(im2.getpixel((x-1,y))),
                to_gray(im2.getpixel((x+1,y-1))),
                to_gray(im2.getpixel((x-1,y+1))),
                to_gray(im2.getpixel((x,y+1))),
                to_gray(im2.getpixel((x+1,y+1))),
            ]
            is_minima = True
            g = to_gray(im2.getpixel((x,y)))
            for v in nei:
                if g >= v:
                    is_minima = False
                    break
            if is_minima:
                im3.putpixel((x, y), (255, 0,0))
                local_minima.add((x, y))
                marked_pxls.add((x, y))
    
    im3.save(fn_base + ".init_local_minima.bmp")
    
    
    # valley following 1, top->bottom, left->right
    for y in range(1, im.height - 1):
        for x in range(1, im.width - 1):
            if (x, y) in marked_pxls:
                pass
    
    return im2



def run_test(start_img_fn):
    fn_base = os.path.splitext(start_img_fn)[0]
    # pick out trees
    im = Image.open(start_img_fn)
    im_copy = im.copy()
    imdraw = ImageDraw.Draw(im_copy)
    imdraw.rectangle((0, 0, im_copy.width, im_copy.height), fill=init_fill_color)

    markers = []
    with open(fn_base + ".tree_marker.txt") as f:
        for l in f.readlines():
            l = l.strip()
            if l.startswith("TREE") or l.startswith("NOT_TREE"):
                markers += l,
            elif l.startswith("REMOVE"):
                s = l[l.index(" "):]
                remove_ok = False
                for idx in range(len(markers) - 1, -1, -1):
                    l2 = markers[idx]
                    s2 = l2[l2.index(" "):]
                    if s == s2:
                        del markers[idx]
                        remove_ok = True
                        break
                assert remove_ok

    # boxes = []
    draw = ImageDraw.Draw(im)
    for m in markers:
        sp = m.split()
        x = int(float(sp[1]) * im.width + 0.01)
        y = int(float(sp[2]) * im.height + 0.01)
        w = int(float(sp[3]) * im.width + 0.01)
        h = int(float(sp[4]) * im.height + 0.01)
        if sp[0] == "TREE":
            # boxes += ("TREE", x, y, w, h),
            crp = im.crop((x, y, x+w, y+h))
            im_copy.paste(crp, (x, y))
            pass
        elif sp[0] == "NOT_TREE":
            # boxes += ("NOT_TREE", x, y, w, h),
            draw.rectangle(x, y, x+w, y+h, fill=(255,255,255))

    im_copy.save(fn_base + ".tree_only.bmp")
    
    phase_i_init(im_copy, fn_base)


run_test("test-data/tree-valley-following-1.jpg")
run_test("test-data/tree-valley-following-2.jpg")
run_test("test-data/tree-valley-following-3.jpg")
