#!/usr/bin/env python

from __future__ import print_function

primes = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97]

N = 100
seen = set()
for a in range(2, N + 1):
    oa = a
    pa = [0] * 25
    for i in range(25):
        while a % primes[i] == 0:
            pa[i] += 1
            a /= primes[i]
    ta = tuple(pa)
    for b in range(2, N + 1):
        tb = tuple([b * x for x in ta])
        seen.add(tb)
print(len(seen))
