from __future__ import print_function

TILE_TYPE_NOT_TREE = 1
TILE_TYPE_TREE = 2

# for 0x1500 precision orthoimages
TILE_STEP_X = 7
TILE_STEP_Y = 7
TILE_SIZES = [7] # [7. 14, 28]

TRAINING_BATCH_SIZE = 500
TEST_BATCHES = 4  # per group, so total (tree + non_tree)is 2 * TRAINING_BATCH_SIZE * TEST_BATCHES

TRAINING_COMBINATIONS = [
    ("IR",),
    ("LAB_A",),
    ("CMYK_Y",),
    # ("LAS_Z_STDDEV_5",),
    # ("LAS_Z_STDDEV_10",),
    # ("LAS_Z_STDDEV_15",),
    ("LAS_Z_STDDEV_20",),
    # ("LAS_Z_STDDEV_25",),
    ("R", "G", "B"),
    ("R", "G", "B", "IR", "LAB_A", "HSB_S", "CMYK_Y", "LAS_Z_STDDEV_20"),
]
