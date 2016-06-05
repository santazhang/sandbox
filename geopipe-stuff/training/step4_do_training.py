#!/usr/bin/env python

from __future__ import print_function

import os
os.chdir(os.path.dirname(__file__))

from training_configurations import *

for channels in TRAINING_COMBINATIONS:
    config_name = "-".join(channels)
    print(config_name)
    if not os.path.exists("06_training_output/" + config_name):
        os.makedirs("06_training_output/" + config_name)
