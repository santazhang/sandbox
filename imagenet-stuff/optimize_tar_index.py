#!/usr/bin/env python

from __future__ import print_function

import sys

def main():
    for fn in sys.argv[1:]:
        if not fn.endswith(".index"):
            print("Unsupported: %s" % fn)
            continue
        print("Optimize %s" % fn)

        index_info = []
        with open(fn, "r") as f:
            while True:
                line = f.readline()
                if line == "":
                    break
                sp = line.split()
                index_info += tuple(sp),
        field1_len = max([len(x[0]) for x in index_info])
        field2_len = max([len(x[1]) for x in index_info])
        field3_len = max([len(x[2]) for x in index_info])
        fmt = "%%-%ds %%%ds %%%ds\n" % (field1_len, field2_len, field3_len)
        with open(fn + "_opt", "w") as f:
            for x in index_info:
                f.write(fmt % x)

if __name__ == "__main__":
    main()
