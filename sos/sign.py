#!/usr/bin/env python
# Mark the boot sector as bootable.

import sys

if len(sys.argv) < 2:
  print "Usage: ./sign.py <boot sector name>"
  exit()

f = open(sys.argv[1], "r+w")
b = f.read()
if len(b) > 510:
  print "ERROR: boot block too large! %d bytes (max 510)" % len(b)
  exit()
print "INFO: boot block size = %d" % len(b)
b += "\0" * (510 - len(b))
b += "\x55\xAA"
f.seek(0)
f.write(b)
f.close()
print "SUCCESS: boot block signed"
