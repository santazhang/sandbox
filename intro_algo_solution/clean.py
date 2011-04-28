#!/usr/bin/python

# This script is used to cleanup the generarted files.

import os

def my_exec(cmd):
  print "[cmd] %s" % cmd
  os.system(cmd)

my_exec("rm -f *.log *.swp *.toc *.idx *.nav *.out *.snm *.dvi *.aux *.synctex.gz")

for f in os.listdir("."):
  if f.endswith(".pdf"):
    tex_fn = f[:-4] + ".tex"
    if os.path.exists(tex_fn):
      my_exec("rm -f %s" % f)
