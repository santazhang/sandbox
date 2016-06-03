#!/usr/bin/env python

from PIL import Image
import os
import subprocess

for fn in os.listdir("/Users/santa/Downloads/tree-slow-test/"):
    if fn.startswith("."):
        continue
    if not fn.endswith(".bmp"):
        continue
    test_in_bmp = "/Users/santa/Downloads/tree-slow-test/" + fn
    
    out_tree_mark_txt = os.path.splitext(test_in_bmp)[0] + ".tree_marker.txt"
    in_bmp = Image.open(test_in_bmp)

    with open(out_tree_mark_txt, "w") as out_mark_f:

        for y in range(0, in_bmp.height - 7, 7):
            print("%.2f%%" % (100.0 * y / in_bmp.height))
            for x in range(0, in_bmp.width - 7, 7):
                fx = 1.0 * x / in_bmp.width
                fy = 1.0 * y / in_bmp.height
                fw = 7.0 / in_bmp.width
                fh = 7.0 / in_bmp.height

                im2 = in_bmp.crop((x, y, x + 7, y + 7))
                im2.save("p.bmp")
                p = subprocess.Popen("./run-predict.sh", shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                p.wait()
                for l in p.stderr.read().split("\n"):
                    l = l.strip()
                    if l.endswith("result -> NOT_TREE"):
                        print("NOT TREE %f %f %f %f" % (fx, fy, fw, fh))
                    elif l.endswith("result -> TREE"):
                        print("TREE %f %f %f %f" % (fx, fy, fw, fh))
                        out_mark_f.write("TREE %f %f %f %f\n" % (fx, fy, fw, fh))
    
