#!/usr/bin/env python

from __future__ import print_function

import math

f = open("p099_base_exp.txt")

line_no = 1
values = []
for line in f.readlines():
    line = line.strip()
    sp = line.split(",")
    b, e = int(sp[0]), int(sp[1])
    values += (math.log(b) * e, b, e, line_no),
    line_no += 1

print(sorted(values, reverse=True)[0])
