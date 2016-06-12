#!/usr/bin/env python

from __future__ import print_function

import os
os.chdir(os.path.dirname(__file__))

from utils import *

for d1 in os.listdir("06_training_output/"):
    retrain_script = os.path.join("06_training_output/", d1, "retrain.sh")
    test_script = os.path.join("06_training_output/", d1, "test.sh")
    if os.path.exists(retrain_script):
        run_cmd(retrain_script)
    if os.path.exists(test_script):
        run_cmd(test_script)
