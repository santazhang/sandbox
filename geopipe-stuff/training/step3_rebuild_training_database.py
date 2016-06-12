#!/usr/bin/env python

from __future__ import print_function

import os
os.chdir(os.path.dirname(__file__))

import random
from training_configurations import *
from utils import *

for channels in TRAINING_COMBINATIONS:
    config_name = "-".join(channels)
    if config_name not in ["R-G-B", "IR"]:
        print("  *** NOT SUPPORTED YET: %s" % config_name)
        continue
    print("Building training data for: %s" % config_name)
    for tile_size in TILE_SIZES:
        print("Tile size: %dx%d" % (tile_size, tile_size))
        if config_name == "R-G-B":
            channels = "rgb"
        elif config_name == "IR":
            channels = "ir"
        else:
            continue
        tree_tile_dir = "04_training_data_preview/tree_%dx%d_%s" % (tile_size, tile_size, channels)
        not_tree_tile_dir = "04_training_data_preview/not_tree_%dx%d_%s" % (tile_size, tile_size, channels)
        tree_data_files = []
        not_tree_data_files = []
        for root, dnames, fnames in os.walk(tree_tile_dir):
            for fn in fnames:
                if fn.startswith(".") or not fn.endswith(".png"):
                    continue
                fpath = os.path.abspath(os.path.join(root, fn))
                tree_data_files += fpath,
        for root, dnames, fnames in os.walk(not_tree_tile_dir):
            for fn in fnames:
                if fn.startswith(".") or not fn.endswith(".png"):
                    continue
                fpath = os.path.abspath(os.path.join(root, fn))
                not_tree_data_files += fpath,
        per_group_image_samples = min(len(not_tree_data_files), len(tree_data_files))
        per_group_image_samples = per_group_image_samples / TRAINING_BATCH_SIZE * TRAINING_BATCH_SIZE
        num_test_images = TEST_BATCHES * TRAINING_BATCH_SIZE
        if per_group_image_samples < 2 * num_test_images:
            print("  *** Too few training sample images found (%d, at least %d wanted)" % (per_group_image_samples, 2 * num_test_images))
            continue
        print("Choose %d images (for each group) from %d tree tiles and %d not-tree tiles" % (per_group_image_samples, len(not_tree_data_files), len(tree_data_files)))
        random.shuffle(not_tree_data_files)
        random.shuffle(tree_data_files)
        not_tree_data_files = not_tree_data_files[:per_group_image_samples]
        tree_data_files = tree_data_files[:per_group_image_samples]

        test_list_txt_fn = "05_training_data/%dx%d-" % (tile_size, tile_size) + config_name + ".test-list.txt"
        with open(test_list_txt_fn, "w") as f:
            for x in not_tree_data_files[:num_test_images]:
                f.write(x)
                f.write(" %d\n" % TILE_TYPE_NOT_TREE)
            for x in tree_data_files[:num_test_images]:
                f.write(x)
                f.write(" %d\n" % TILE_TYPE_TREE)

        train_list_txt_fn = "05_training_data/%dx%d-" % (tile_size, tile_size) + config_name + ".train-list.txt"
        with open(train_list_txt_fn, "w") as f:
            for x in not_tree_data_files[num_test_images:]:
                f.write(x)
                f.write(" %d\n" % TILE_TYPE_NOT_TREE)
            for x in tree_data_files[num_test_images:]:
                f.write(x)
                f.write(" %d\n" % TILE_TYPE_TREE)

        if config_name == "R-G-B":
            use_gray_if_necessary = ""
        elif config_name == "IR":
            use_gray_if_necessary = " --gray"
        else:
            print("  *** NOT SUPPORTED YET: %s" % config_name)
            continue

        run_cmd("rm -rfv '05_training_data/%dx%d-%s-test-lmdb/'" % (tile_size, tile_size, config_name))
        run_cmd("rm -rfv '05_training_data/%dx%d-%s-train-lmdb/'" % (tile_size, tile_size, config_name))

        run_cmd("../deps/_build/caffe/build/tools/convert_imageset --backend=lmdb --shuffle " + use_gray_if_necessary +
                "/ %s 05_training_data/%dx%d-%s-test-lmdb/" % (test_list_txt_fn, tile_size, tile_size, config_name))
        run_cmd("../deps/_build/caffe/build/tools/convert_imageset --backend=lmdb --shuffle " + use_gray_if_necessary +
                "/ %s 05_training_data/%dx%d-%s-train-lmdb/" % (train_list_txt_fn, tile_size, tile_size, config_name))

        os.remove(test_list_txt_fn)
        os.remove(train_list_txt_fn)
