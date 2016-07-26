#!/usr/bin/env python

import math

with open("p042_words.txt") as f:
    words = [w.strip('"') for w in f.read().split('","')]

cnt = 0

for w in words:
    x = 0
    for c in w:
        x += ord(c) - ord('A') + 1
    y = math.sqrt(1+8*x)
    if y - math.floor(y) > 0.01:
        continue
    if int(y) % 2 == 1:
        cnt += 1

print(cnt)
