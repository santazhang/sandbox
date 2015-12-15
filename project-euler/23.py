#!/usr/bin/env python

from __future__ import print_function

upper_limit = 28123

d = {}

for a in range(0, upper_limit + 1):
    d[a] = set()

abundant = []

numbers = set()

for a in range(1, upper_limit + 1):
    s = sum(d[a])
    if s > a:
        abundant += a,
        print(a, d[a])
    for b in range(2 * a, upper_limit + 1, a):
        d[b].add(a)

for a in abundant:
    for b in abundant:
        numbers.add(a + b)

s = 0
for i in range(28123):
    if i not in numbers:
        print(i)
        s += i

print("sum", s)
