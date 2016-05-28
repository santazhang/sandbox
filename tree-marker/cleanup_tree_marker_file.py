#!/usr/bin/env python

import sys
import os

def cleanup(fname):
    print("cleanup: %s" % fname)
    markers = []
    with open(fname) as f:
        for l in f.readlines():
            l = l.strip()
            if l.startswith("TREE") or l.startswith("NOT_TREE"):
                markers += l,
            elif l.startswith("REMOVE"):
                s = l[l.index(" "):]
                remove_ok = False
                for idx in range(len(markers) - 1, -1, -1):
                    l2 = markers[idx]
                    s2 = l2[l2.index(" "):]
                    if s == s2:
                        del markers[idx]
                        remove_ok = True
                        break
                assert remove_ok

    with open(fname + "._cleanup_", "w") as g:
        for m in markers:
            g.write(m)
            g.write("\n")
    
    os.rename(fname + "._cleanup_", fname)


if __name__ == "__main__":
    for fn in sys.argv[1:]:
        cleanup(fn)
