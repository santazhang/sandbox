#!/usr/bin/env python

from __future__ import print_function

import math

n = 0
while True:
    n += 1
    Hn = n * (2 * n - 1)
    g = int((1 + math.sqrt(1 + 24 * Hn)) / 6)
    found = False
    for x in [g - 1, g, g + 1]:
        y = x * (3 * x - 1) / 2
        if y == Hn:
            found = True
    if not found:
        continue
    g = int((-1 + math.sqrt(1 + 8 * Hn)) / 2)
    found = False
    for x in [g - 1, g, g + 1]:
        y = x * (x + 1) / 2
        if y == Hn:
            found = True
    if found:
        print("H(%d)=%d" % (n, Hn))
        if n > 143:
            break
