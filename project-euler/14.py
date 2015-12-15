#!/usr/bin/env python

from __future__ import print_function

d = {}

d[1] = 1

m = 1

for x in range(1, 1000 * 1000 + 1):
    l = 0
    n = x
    while True:
        if n in d:
            d[x] = l + d[n]
            break
        l += 1
        if n % 2 == 0:
            n = n / 2
        else:
            n = 3 * n + 1
    if d[x] > d[m]:
        m = x
        print(x, "chain len =", d[x])
