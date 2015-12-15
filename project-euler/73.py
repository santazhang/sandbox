#!/usr/bin/env python

from __future__ import print_function

fractions = set()

def gcd(a, b):
    if a < b:
        a, b = b, a
    while True:
        r = a % b
        if r == 0:
            return b
        a, b = b, r

for d in range(2, 12000 + 1):
    print(d)
    for n in range(1, d):
        g = gcd(n, d)
        t = (n / g, d / g)
        fractions.add(t)

count = 0
for t in fractions:
    n, d = t
    if d > 2 * n and 3 * n > d:
        count += 1
print("count =", count)
