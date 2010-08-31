#!/usr/bin/python

import os
import random

def rand_bigint_str_positive():
  if random.randint(0, 1) == 0:
    # generate big value
    digits = random.randint(1, 1000)
    val = "0"
    for i in range(digits):
      val += "0123456789"[random.randint(0, 9)]
    return val
  else:
    # generate small value
    return str(random.randint(0, 1000000000000000000000000L))

def rand_bigint_str():
  if random.randint(0, 1) == 0:
    return rand_bigint_str_positive()
  else:
    return "-" + rand_bigint_str_positive();

os.system("rm -f test")
os.system("gcc bigint.c test.c -O3 -o test -lm")

op_list = ["+", "-", "time", "div", "pow", "mod"]

for i in range(10000):
  a = rand_bigint_str()
  va = int(a)
  b = rand_bigint_str()
  vb = int(b)
  op = op_list[random.randint(0, len(op_list) - 1)]
  if op == "+":
    v = va + vb
  elif op == "-":
    v = va - vb
  elif op == "times":
    v = va * vb
  elif op == "div":
    v = va / vb
  elif op == "pow":
    vb = vb % 100
    v = va ** vb
  elif op == "mod":
    v = va % vb
  cmd = "./test %d %s %d" % (va, op, vb)
  pipe = os.popen(cmd)
  line = pipe.readline()
  if line.startswith("va ="):
    sp = line.split()
    vc = int(sp[2])
    if vc != v:
      print "calc error, %d %s %d\n" % (va, op, vb)
      print "%d (bigint)\n!=\n%d (python)" % (vc, v)
    else:
      print "calc ok"
  else:
    print "exec error"
  pipe.close()

