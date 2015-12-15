#!/usr/bin/env python

from __future__ import print_function

def P(m):
    x = 1
    for i in range(1, m + 1):
        x *= (i * 2.0 / (m + 1)) ** i
    return x

print(sum([int(P(m)) for m in range(2, 16)]))
