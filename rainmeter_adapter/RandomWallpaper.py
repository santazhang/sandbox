#!/usr/bin/python

import random
import os

def random_wallpaper(imglist):
    if len(imglist) == 0:
        raise Exception("Empty image list!")
    idx = random.randint(0, len(imglist) - 1)
    wp = imglist[idx]
    cmd = "gconftool-2 -t string -s /desktop/gnome/background/picture_filename '%s'" % wp
    print cmd
    os.system(cmd)

def all_images_in_folder(folder):
    imglist = os.listdir(folder)
    imglist = map(lambda f: folder + os.path.sep + f, imglist)
    imglist2 = []
    for img in imglist:
        if img.lower().endswith(".jpg") or img.lower().endswith(".png"):
            imglist2 += img,
    return imglist2

if __name__ == "__main__":
    imglist = all_images_in_folder("/home/santa/Pictures/Best Collection")
    random_wallpaper(imglist)

