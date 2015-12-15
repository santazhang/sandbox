#!/usr/bin/env python

from __future__ import print_function

import math

def gcd(a, b):
    if a < b:
        a, b = b, a
    while True:
        r = a % b
        if r == 0:
            return b
        a, b = b, r

def continued_fraction(N):
    sqrtN = math.sqrt(N)
    x = int(sqrtN)
    if x * x == N:
        return [[x], []]
    a, b, c = 1, 0, 1
    l = []
    seen = []
    while True:
        x = int((a * sqrtN + b) / c)
        t = (x, a, b, c)
        if t in seen:
            idx = seen.index(t)
            return [[v[0] for v in seen[:idx]],
                    [v[0] for v in seen[idx:]]]
        seen += t,
        a1 = a * c
        b1 = c * c * x - b * c
        c1 = a * a * N - (b - c * x) * (b - c * x)
        g = gcd(a1, gcd(b1, c1))
        a, b, c = a1 / g, b1 / g, c1 / g
        l += [x]

count = 0
for i in range(10000 + 1):
    cf = continued_fraction(i)
    print("sqrt", i, "->", cf)
    if len(cf[1]) % 2 == 1:
        count += 1
print("count =", count)
