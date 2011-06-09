#!/usr/bin/env python

import csv


def sel_score(splt):  
  # programming assignment
  col_prog = 11

  # number of choices
  n_sel = 6
  
  ret = splt[0:2]
  
  # the scores to mark
  sels = splt[2:col_prog]
  print sels
  
  sels2 = []
  for i in range(len(sels)):
    sc = sels[i]
    if sc == "":
      sc = 0
    else:
      sc = int(sc)
    sels2 += (sc, i),
  print sels2
  sels2.sort()
  sels2.reverse()
  print sels2
  for i in range(n_sel):
    idx = sels2[i][1] + 2
    print "idx = ", idx
    print "splt = ", splt[idx]
    if splt[idx] != '':
      splt[idx] = "*" + splt[idx]
  
  
  ret += splt[2:col_prog]
  if splt[col_prog] != "":
    splt[col_prog] = "*" + splt[col_prog]
  ret += splt[col_prog],
  
  final = 0
  for i in range(2, len(splt)):
    idx = i
    if splt[i].startswith("*"):
      val = int(splt[i][1:])
      print "val = %d" % val
      final += val
  final = final / 2.0
  ret += "=" + str(final), 
  ret2 = [ret[0],] + ret[2:]
  return ret2
  
  
  

f = open("algo-score.csv")
g = open("algo-score-marked.csv", "w")

lines = f.readlines()
lines = lines[1:]


for line in lines:
  line = line.strip()
  splt = map(str.strip, line.split(','))
  print splt
  ret = sel_score(splt)
  print ret
  print
  for item in ret:
    g.write(item + ", ")
  g.write("\n")

f.close()
g.close()

