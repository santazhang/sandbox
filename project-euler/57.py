#!/usr/bin/env python

from __future__ import print_function

a, b = 3, 2

count = 0
for i in range(1000):
    if len(str(a)) > len(str(b)):
        count += 1
    a, b = a + 2 * b, a + b
print(count)
