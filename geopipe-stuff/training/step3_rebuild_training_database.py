#!/usr/bin/env python

from __future__ import print_function

import os
os.chdir(os.path.dirname(__file__))

import random
from training_configurations import *
from utils import *

RAND_SEED_SELECT_DATA = os.getpid() + 830231
RAND_SEED_FOR_TEST = os.getpid() + 281943
RAND_SEED_FOR_TRAIN = os.getpid() + 938184


for tile_size in TILE_SIZES:
    allowed_fnames = set()
    store_fnames_list = {}
    print("Tile size: %dx%d" % (tile_size, tile_size))

    for channels in TRAINING_COMBINATIONS:
        config_name = "-".join(channels)
        if config_name == "R-G-B":
            channels = "rgb"
        elif config_name == "IR":
            channels = "ir"
        elif config_name == "LAB_A":
            channels = "lab_a"
        elif config_name == "CMYK_Y":
            channels = "cmyk_y"
        else:
            print("  *** NOT SUPPORTED YET: %s" % config_name)
            continue

        print("Scanning training data for: %s (tile size: %dx%d)" % (config_name, tile_size, tile_size))
        tree_tile_dir = "04_training_data_preview/tree_%dx%d_%s" % (tile_size, tile_size, channels)
        not_tree_tile_dir = "04_training_data_preview/not_tree_%dx%d_%s" % (tile_size, tile_size, channels)
        tree_data_files = []
        not_tree_data_files = []
        image_fnames_set = set()
        for root, dnames, fnames in os.walk(tree_tile_dir):
            for fn in fnames:
                if fn.startswith(".") or not fn.endswith(".png"):
                    continue
                fpath = os.path.abspath(os.path.join(root, fn))
                tree_data_files += fpath,
                image_fnames_set.add(fn)
        for root, dnames, fnames in os.walk(not_tree_tile_dir):
            for fn in fnames:
                if fn.startswith(".") or not fn.endswith(".png"):
                    continue
                fpath = os.path.abspath(os.path.join(root, fn))
                not_tree_data_files += fpath,
                image_fnames_set.add(fn)
        if len(allowed_fnames) == 0:
            allowed_fnames = image_fnames_set
        else:
            allowed_fnames = allowed_fnames.intersection(image_fnames_set)
        print("%d tree and %d not_tree files scanned" % (len(tree_data_files), len(not_tree_data_files)))
        store_fnames_list["tree~" + config_name] = tree_data_files
        store_fnames_list["not_tree~" + config_name] = not_tree_data_files


    for channels in TRAINING_COMBINATIONS:
        config_name = "-".join(channels)
        if config_name == "R-G-B":
            channels = "rgb"
        elif config_name == "IR":
            channels = "ir"
        elif config_name == "LAB_A":
            channels = "lab_a"
        elif config_name == "CMYK_Y":
            channels = "cmyk_y"
        else:
            print("  *** NOT SUPPORTED YET: %s" % config_name)
            continue

        print("Building training data for: %s (tile size: %dx%d)" % (config_name, tile_size, tile_size))
        tree_data_files = store_fnames_list["tree~" + config_name]
        not_tree_data_files = store_fnames_list["not_tree~" + config_name]

        ignore_image_cnt = 0
        for L in [tree_data_files, not_tree_data_files]:
            idx = 0
            while idx < len(L):
                fn = os.path.basename(L[idx])
                if fn not in allowed_fnames:
                    ignore_image_cnt += 1
                    L[idx] = L[-1]
                    del L[-1]
                    continue
                idx += 1
        if ignore_image_cnt > 0:
            print("Ignored %d images" % ignore_image_cnt)
        tree_data_files.sort()
        not_tree_data_files.sort()

        per_group_image_samples = min(len(not_tree_data_files), len(tree_data_files))
        per_group_image_samples = per_group_image_samples / TRAINING_BATCH_SIZE * TRAINING_BATCH_SIZE
        num_test_images = TEST_BATCHES * TRAINING_BATCH_SIZE
        if per_group_image_samples < 2 * num_test_images:
            print("  *** Too few training sample images found (%d, at least %d wanted)" % (per_group_image_samples, 2 * num_test_images))
            continue
        print("Choose %d images (for each group) from %d not-tree tiles and %d tree tiles" % (per_group_image_samples, len(not_tree_data_files), len(tree_data_files)))
        random.seed(RAND_SEED_SELECT_DATA)
        random.shuffle(not_tree_data_files)
        random.shuffle(tree_data_files)
        not_tree_data_files = not_tree_data_files[:per_group_image_samples]
        tree_data_files = tree_data_files[:per_group_image_samples]

        random.seed(RAND_SEED_FOR_TEST)
        test_list_txt_fn = "05_training_data/%dx%d-" % (tile_size, tile_size) + config_name + ".test-list.txt"
        with open(test_list_txt_fn, "w") as f:
            out = []
            for x in not_tree_data_files[:num_test_images]:
                out += x + " 0\n",
            for x in tree_data_files[:num_test_images]:
                out += x + " 1\n",
            random.shuffle(out)
            for x in out:
                f.write(x)

        random.seed(RAND_SEED_FOR_TRAIN)
        train_list_txt_fn = "05_training_data/%dx%d-" % (tile_size, tile_size) + config_name + ".train-list.txt"
        with open(train_list_txt_fn, "w") as f:
            out = []
            for x in not_tree_data_files[num_test_images:]:
                out += x + " 0\n",
            for x in tree_data_files[num_test_images:]:
                out += x + " 1\n",
            random.shuffle(out)
            for x in out:
                f.write(x)

        if config_name == "R-G-B":
            use_gray_if_necessary = ""
        elif config_name in ["IR", "LAB_A", "CMYK_Y"]:
            use_gray_if_necessary = " --gray"
        else:
            print("  *** NOT SUPPORTED YET: %s" % config_name)
            continue

        run_cmd("rm -rfv '05_training_data/%dx%d-%s-test-lmdb/'" % (tile_size, tile_size, config_name))
        run_cmd("rm -rfv '05_training_data/%dx%d-%s-train-lmdb/'" % (tile_size, tile_size, config_name))

        run_cmd("../deps/_build/caffe/build/tools/convert_imageset --backend=lmdb " + use_gray_if_necessary +
                " / %s 05_training_data/%dx%d-%s-test-lmdb/" % (test_list_txt_fn, tile_size, tile_size, config_name))
        run_cmd("../deps/_build/caffe/build/tools/convert_imageset --backend=lmdb " + use_gray_if_necessary +
                " / %s 05_training_data/%dx%d-%s-train-lmdb/" % (train_list_txt_fn, tile_size, tile_size, config_name))

        # os.remove(test_list_txt_fn)
        # os.remove(train_list_txt_fn)

        # Build las z_hint training info. Using RGB data set (which is gauranteed to be built) to build z_hint data set.
        if config_name == "R-G-B":
            all_las_z_hint_found = True

            def extract_las_info(img_fpath):
                # img_fpath eg: .../18TWL835090_07_0007_4886.png
                main_img_fn = os.path.splitext(os.path.basename(img_fpath))[0]
                sp = main_img_fn.split("_")
                las_fpath = os.path.abspath(os.path.join("02_decoded_data", sp[0] + ".las.0x1500m.z_hint"))
                if not os.path.exists(las_fpath):
                    all_las_z_hint_found = False
                    return None
                las_x, las_y = int(sp[2]), int(sp[3])
                return las_fpath, las_x, las_y

            test_list_txt_fn = "05_training_data/%dx%d-" % (tile_size, tile_size) + "LAS_Z_HINT.test-list.txt"
            random.seed(RAND_SEED_FOR_TEST)
            with open(test_list_txt_fn, "w") as f:
                out = []  # (fpath, x, y, w, h, label)
                for fpath in not_tree_data_files[:num_test_images]:
                    ex = extract_las_info(fpath)
                    out += (ex[0], ex[1], ex[2], tile_size, tile_size, 0),
                for fpath in tree_data_files[:num_test_images]:
                    ex = extract_las_info(fpath)
                    out += (ex[0], ex[1], ex[2], tile_size, tile_size, 1),
                random.shuffle(out)
                for item in out:
                    f.write("%s %d %d %d %d %d\n" % item)

            train_list_txt_fn = "05_training_data/%dx%d-" % (tile_size, tile_size) + "LAS_Z_HINT.train-list.txt"
            random.seed(RAND_SEED_FOR_TRAIN)
            with open(train_list_txt_fn, "w") as f:
                out = []  # (fpath, x, y, w, h, label)
                for fpath in not_tree_data_files[num_test_images:]:
                    ex = extract_las_info(fpath)
                    out += (ex[0], ex[1], ex[2], tile_size, tile_size, 0),
                for fpath in tree_data_files[num_test_images:]:
                    ex = extract_las_info(fpath)
                    out += (ex[0], ex[1], ex[2], tile_size, tile_size, 1),
                random.shuffle(out)
                for item in out:
                    f.write("%s %d %d %d %d %d\n" % item)

            # las data not found for some images, so RGB data and las z_hint does not line up -> do not build z hint data
            if not all_las_z_hint_found:
                os.remove(test_list_txt_fn)
                os.remove(train_list_txt_fn)
                print("  *** LAS and RGB data not aligned, skip building LAS database!")
                continue

            run_cmd("rm -rfv '05_training_data/%dx%d-LAS_Z_HINT-test-lmdb/'" % (tile_size, tile_size))
            run_cmd("rm -rfv '05_training_data/%dx%d-LAS_Z_HINT-train-lmdb/'" % (tile_size, tile_size))
            run_cmd("../scripts/run_activated.sh ../build/build_z_hint_db " +
                    " / %s 05_training_data/%dx%d-LAS_Z_HINT-test-lmdb/" % (test_list_txt_fn, tile_size, tile_size))
            run_cmd("../scripts/run_activated.sh ../build/build_z_hint_db " +
                    " / %s 05_training_data/%dx%d-LAS_Z_HINT-train-lmdb/" % (train_list_txt_fn, tile_size, tile_size))
