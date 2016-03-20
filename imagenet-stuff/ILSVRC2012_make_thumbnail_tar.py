#!/usr/bin/env python

from __future__ import print_function

import os
import sys

THUMBNAIL_WIDTH = 256
THUMBNAIL_HEIGHT = 256

def runcmd(cmd):
    print("+ %s" % cmd)
    return os.system(cmd)

def main():
    if len(sys.argv) < 3:
        print("Usage: %s image-tar work-dir" % sys.argv[0])
        exit(1)
    image_tar, work_dir = sys.argv[1], sys.argv[2]
    if os.path.exists(work_dir):
        print("%s already exists!" % work_dir)
        exit(1)
    data_dir = os.path.join(work_dir, "data")
    os.makedirs(data_dir)
    image_index = os.path.splitext(image_tar)[0] + ".index"
    if not os.path.exists(image_index):
        print("image index file %s not found!" % image_index)
        exit(1)
    image_main_fn = os.path.splitext(os.path.basename(image_tar))[0]
    thumb_suffix = "_thumbnail_%dx%d" % (THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT)
    thumb_tar = os.path.join(work_dir, image_main_fn + thumb_suffix + ".tar")
    thumb_index = os.path.join(work_dir, image_main_fn + thumb_suffix + ".index")
    thumb_index2 = os.path.join(work_dir, image_main_fn + thumb_suffix + ".index2")

    with open(image_tar, "rb") as image_tar_f:
        with open(image_index, "r") as image_index_f:
            while True:
                line = image_index_f.readline()
                if line == "":
                    break
                sp = line.split()
                fn, offst, sz = os.path.join(data_dir, sp[0]), int(sp[1]), int(sp[2])
                print("Extract %s" % fn)
                image_tar_f.seek(offst)
                with open(fn, "wb") as f:
                    cnt = 0
                    while cnt < sz:
                        image_data = image_tar_f.read(min(4096, sz - cnt))
                        f.write(image_data)
                        cnt += len(image_data)
                thumb_fn = os.path.splitext(fn)[0] + thumb_suffix + os.path.splitext(fn)[1]
                print("Convert %s" % thumb_fn)
                cmd = "convert -resize %dx%d! '%s' '%s'" % (
                        THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, fn, thumb_fn)
                if runcmd(cmd) != 0:
                    exit(1)
                os.remove(fn)

    files_list = os.path.join(work_dir, "files.list")
    runcmd("cd '%s' ; ls -1 > %s ; tar --files-from='%s' -cf '%s'" % (data_dir, files_list, files_list, thumb_tar))
    runcmd("python tarindexer.py -i '%s' '%s'" % (thumb_tar, thumb_index2))
    thumb_idx = []
    with open(thumb_index2, "r") as thumb_index2_f:
        while True:
            line = thumb_index2_f.readline()
            if line == "":
                break
            sp = line.split()
            thumb_idx += tuple(sp),
    field1_len = max([len(x[0]) for x in thumb_idx])
    field2_len = max([len(x[1]) for x in thumb_idx])
    field3_len = max([len(x[2]) for x in thumb_idx])
    fmt = "%%-%ds %%%ds %%%ds\n" % (field1_len, field2_len, field3_len)
    with open(thumb_index, "w") as thumb_index_f:
        for x in thumb_idx:
            thumb_index_f.write(fmt % x)
    os.remove(thumb_index2)

if __name__ == "__main__":
    main()
