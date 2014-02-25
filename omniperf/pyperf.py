#!/usr/bin/env python

import time

start = time.time()
now = time.time()
ctr = 0
while True:
    now = time.time()
    if now - start > 1.0:
        break
    for i in range(100000):
        ctr += 1

print "int++ {:,.0f}/s".format(ctr / (now - start))

start = time.time()
now = time.time()
ctr = 0
while True:
    now = time.time()
    if now - start > 1.0:
        break
    for i in range(100000):
        ctr -= 1
ctr = -ctr
print "int-- {:,.0f}/s".format(ctr / (now - start))

start = time.time()
now = time.time()
d = {}
ctr = 0
while True:
    now = time.time()
    if now - start > 1.0:
        break
    for i in range(10000):
        d[ctr] = ctr
    ctr += 10000

print "dict[key]=value (int-int) assignment {:,.0f}/s".format(ctr / (now - start))

start = time.time()
now = time.time()
ctr = 0
while True:
    now = time.time()
    if now - start > 1.0:
        break
    for i in range(10000):
        d[0]
    ctr += 10000

print "dict[key] (int-int) lookup {:,.0f}/s".format(ctr / (now - start))

start = time.time()
now = time.time()
l = []
ctr = 0
while True:
    now = time.time()
    if now - start > 1.0:
        break
    for i in range(10000):
        l += ctr,
    ctr += 10000

print "list (int) append {:,.0f}/s".format(ctr / (now - start))
