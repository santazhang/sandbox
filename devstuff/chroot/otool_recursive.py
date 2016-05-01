#!/usr/bin/env python

import os
import sys

checked = set()

def otool_recursive(fpath):
    with os.popen("otool -L '%s'" % fpath) as p:
        out = p.read()
        for l in out.split("\n"):
            if l.startswith("\t"):
                idx = l.index(" (compatibility version")
                l = l[:idx].strip()
                if l not in checked:
                    print(l)
                    checked.add(l)
                    otool_recursive(l)

otool_recursive(sys.argv[1])
