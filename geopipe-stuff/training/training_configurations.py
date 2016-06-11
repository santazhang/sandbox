from __future__ import print_function

TREE_RECOGNITION_RESOLUTION_IN_METERS = 4.0

TRAINING_BATCH_SIZE = 500

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
