#!/usr/bin/env python

from __future__ import print_function

import math

upper = 50 * 1000 * 1000
primes_upper = int(math.sqrt(upper)) + 1

primes = []

is_prime = [True] * primes_upper
is_prime[0] = False
is_prime[1] = False
for i in range(primes_upper):
    if is_prime[i]:
        primes += i,
        print("prime:", i)
        for j in range(i, primes_upper, i):
            is_prime[j] = False

primes3 = []
primes4 = []
for p in primes:
    if p ** 3 < upper:
        primes3 += p,
    if p ** 4 < upper:
        primes4 += p,

values = set()
for a in primes:
    for b in primes3:
        for c in primes4:
            v = a ** 2 + b ** 3 + c ** 4
            if v >= upper:
                continue
            if v not in values:
                #print(v, a, b, c)
                values.add(v)

print("count:", len(values))
