#!/usr/bin/env python

import os
import shutil
import sys

if len(sys.argv) < 2:
  print "usage: simpic_export.py <export_dir>"
  exit(0)

if os.path.exists("simpic.csv") == False:
  print "cannot find 'simpic.csv'! run 'simpic' first!"
  exit(0)

export_dir = sys.argv[1]
f = open("simpic.csv")

f.readline() # skip line 1
pairs = []
for l in f.readlines():
  sp = l.strip().split(",")
  pairs += (float(sp[2]), sp[2], sp[0], sp[1]),
f.close()
pairs.sort()
index = 0
while index < 1000 and index < len(pairs):
  p = pairs[index]
  index += 1
  fn1 = os.path.join(export_dir, p[1] + ("_%da_" % index) + os.path.split(p[2])[1])
  fn2 = os.path.join(export_dir, p[1] + ("_%db_" % index) + os.path.split(p[3])[1])
  print "%s -> %s" % (p[2], fn1)
  shutil.copy(p[2], fn1);
  print "%s -> %s" % (p[3], fn2)
  shutil.copy(p[3], fn2);
  print
