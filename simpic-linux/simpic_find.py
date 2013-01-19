#!/usr/bin/env python

import os
import sys
import math

if len(sys.argv) < 2:
    print "usage: simpic_find.py <folder1> [folder2] ..."
    exit(0)

def is_image(fn):
    fn = fn.lower()
    return fn.endswith(".jpg") or fn.endswith(".jpeg") or fn.endswith(".png") or fn.endswith(".bmp") or fn.endswith(".gif")

g_fp_cache = None
def read_fp_cache(imgfpath):
    global g_fp_cache
    if g_fp_cache == None:
        g_fp_cache = {}
        if os.path.exists("fp_cache.txt"):
            f = open("fp_cache.txt")
            while True:
                line = f.readline()
                if line == "":
                    break
                img_fpath = line.strip()
                line = f.readline()
                sp = line.split()
                imgfp = map(int, sp)
                g_fp_cache[img_fpath] = imgfp
                line = f.readline()
            f.close()
    if imgfpath in g_fp_cache.keys():
        return g_fp_cache[imgfpath]

def write_fp_cache(imgfpath, imgfp):
    global g_fp_cache
    g_fp_cache[imgfpath] = imgfp
    f = open("fp_cache.txt~", "w")
    for k in g_fp_cache:
        f.write("%s\n" % k)
        for v in g_fp_cache[k]:
            f.write("%d " % v)
        f.write("\n\n")
    f.close()
    os.rename("fp_cache.txt~", "fp_cache.txt")

def calc_fp(fpath):
    imgfp = read_fp_cache(fpath)
    if imgfp != None:
        return imgfp

    os.system("convert -resize 250x150 '%s' tmp.png" % fpath)
    p = os.popen("./calc_fp 'tmp.png'", "r")
    out = p.read()
    p.close()
    os.remove("tmp.png")
    sp = out.split()
    if len(sp) == 544:
        imgfp = map(int, sp)
        write_fp_cache(fpath, imgfp)
        return imgfp

image_folders = sys.argv[1:]

print image_folders

all_image_fp = {}

for image_folder in image_folders:
    for root, dirnames, fnames in os.walk(image_folder):
        for fn in fnames:
            if not is_image(fn):
                continue
            fpath = os.path.join(root, fn)
            print "analyzing %s" % fpath
            cvec = calc_fp(fpath)
            if cvec != None:
                all_image_fp[fpath] = cvec

def vec_len(v):
    t = 0
    for e in v:
        t += e * e
    return math.sqrt(t)

def img_diff(v1, v2):
    vdis = []
    for i in range(0, len(v1)):
        # "text fix" in libpuzzle
        c1 = v1[i]
        c2 = v2[i]
        if (c1 == 0 and c2 == -2) or (c1 == -2 and c2 == 0):
            vdis += -3,
        elif (c1 == 0 and c2 == 2) or (c1 == 2 and c2 == 0):
            vdis += 3,
        else:
            vdis += c1 - c2,
    assert len(vdis) == len(v1)
    dis = vec_len(vdis)
    div = vec_len(v1) + vec_len(v2)
    if div < 0.000001:
        return 0.0
    return dis / div

diff_result = []

print "finding similiar images"
for img1 in all_image_fp.keys():
    for img2 in all_image_fp.keys():
        if img1 >= img2:
            continue
        diff = img_diff(all_image_fp[img1], all_image_fp[img2])
        #print "%lf %s %s" % (diff, img1, img2)
        diff_result += (diff, img1, img2),

diff_result.sort()
#print diff_result

diff_limit = 1000
diff_threshold = 0.6

if diff_limit > len(diff_result):
    diff_limit = len(diff_result)

for i in range(0, diff_limit):
    if diff_result[i][0] < diff_threshold:
        print diff_result[i]

# generate html
f = open("result.html", "w")

f.write("<table>\n")

def rela_path(path):
    return os.path.relpath(path, os.getcwd())

for i in range(0, diff_limit):
    res = diff_result[i]
    if not (res[0] < diff_threshold):
        continue
    f.write("<tr><td rowspan=2>%lf</td>\n" % res[0])

    path1 = rela_path(res[1])
    f.write("<td><img alt='%s' width=250 height=150 src='%s'></td>\n" % (path1, path1))
    path2 = rela_path(res[2])
    f.write("<td><img alt='%s' width=250 height=150 src='%s'></td>\n" % (path2, path2))

    f.write("</tr>\n")
    f.write("<tr><td>%s</td><td>%s</td></tr>\n" % (path1, path2));

f.write("</table>\n")
f.close()

