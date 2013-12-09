#!/usr/bin/env python

import tinymathpy
import time

print dir(tinymathpy)

print tinymathpy.gcd(5, 3)

def f(v):
    print v

print tinymathpy.mt_callback(f)

for i in range(100):
    time.sleep(1)
