from __future__ import print_function

import os

def run_cmd(cmd):
    print("+ %s" % cmd)
    os.system(cmd)
