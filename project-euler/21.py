#!/usr/bin/env python

from __future__ import print_function

import math

upper_limit = 10000
max_upper_limit = int(upper_limit * math.log(upper_limit)) + upper_limit

d = {}

for a in range(0, max_upper_limit + 1):
    d[a] = set()

for a in range(1, max_upper_limit + 1):
    for b in range(2 * a, max_upper_limit + 1, a):
        d[b].add(a)

amicable_sum = 0

for a in range(1, upper_limit + 1):
    s = sum(d[a])
    if sum(d[s]) == a and a < s:
        print(a, s)
        amicable_sum += a + s

print(amicable_sum)
