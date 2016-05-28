#!/usr/bin/env python


from PIL import Image
import os

txt_fn = "18TWL835090_201203_0x6000m_4B_1.RGB.tree_marker.txt"
img_fn = "/Users/santa/Downloads/18TWL835090_201203_0x6000m_4B_1.RGB.bmp"

markers = []
with open(txt_fn) as f:
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

im = Image.open(img_fn)


boxes = []
for m in markers:
    sp = m.split()
    x = int(float(sp[1]) * im.width)
    y = int(float(sp[2]) * im.height)
    w = int(float(sp[3]) * im.width)
    h = int(float(sp[4]) * im.height)
    if sp[0] == "TREE":
        boxes += ("TREE", x, y, w, h),
    elif sp[0] == "NOT_TREE":
        boxes += ("NOT_TREE", x, y, w, h),
#        im2 = im.crop((x, y, x+w, y+h))

def box_cover_ratio(x, y, w, h, bx, by, bw, bh):
    cx = max(x, bx)
    cy = max(y, by)
    cx2 = min(x + w, bx + bw)
    cy2 = min(y + h, by + bh)
    cw = cx2 - cx
    ch = cy2 - cy
    area = cw * ch
    return 1.0 * cw * ch / (w * h)

out_fn_prefix = os.path.splitext(os.path.basename(img_fn))[0]

for y in range(0, im.height, 7):
    print("%.2f%%" % (100.0 * y / im.height))
    for x in range(0, im.width, 7):
        max_tree_cover = 0.0
        max_non_tree_cover = 0.0
        for b in boxes:
            bx = b[1]
            by = b[2]
            bw = b[3]
            bh = b[4]
            if x + 7 <= bx or x >= bx + bw or y + 7 <= by or y >= by + bh:
                continue
            r = box_cover_ratio(x, y, 7, 7, bx, by, bw, bh)
            if b[0] == "TREE":
                max_tree_cover = max(max_tree_cover, r)
            elif b[0] == "NOT_TREE":
                max_non_tree_cover = max(max_non_tree_cover, r)
        if max_tree_cover >= 0.5 and max_tree_cover > max_non_tree_cover:
            print("TREE", x, y)
            im2 = im.crop((x, y, x + 7, y + 7))
            im2.save("/tmp/%s-tree-x%d-y%d.png" % (out_fn_prefix, x, y))
        elif max_non_tree_cover >= 0.5 and max_non_tree_cover > max_tree_cover:
            print("NOT_TREE", x, y)
            im2 = im.crop((x, y, x + 7, y + 7))
            im2.save("/tmp/%s-not-tree-x%d-y%d.png" % (out_fn_prefix, x, y))
            