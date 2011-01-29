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
counter = 1
for l in f.readlines():
  sp = l.strip().split(",")
  fn1 = os.path.join(export_dir, str(sp[2]) + ("_%da_" % counter) + os.path.split(sp[0])[1])
  fn2 = os.path.join(export_dir, str(sp[2]) + ("_%db_" % counter) + os.path.split(sp[1])[1])
  print "%s -> %s" % (sp[0], fn1)
  shutil.copy(sp[0], fn1);
  print "%s -> %s" % (sp[1], fn2)
  shutil.copy(sp[1], fn2);
  print
  counter += 1

f.close()

