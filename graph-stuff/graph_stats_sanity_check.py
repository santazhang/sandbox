#!/usr/bin/env python

from __future__ import print_function

import os
import sys

def main():
    if len(sys.argv) < 2:
        print("Usage: %s graph-stats-file" % sys.argv[0])
        exit(1)
    IN, OUT, IN_OUT = 0, 1, 2
    deg_maps = [{0:0}, {0:0}, {0:0}]  # in, out, in_out
    cur_deg_map = None
    with open(sys.argv[1]) as f:
        while True:
            line = f.readline()
            if line == "":
                break
            line = line.strip()
            if line.startswith("Nodes: "):
                nodes = int(line.split()[1])
            elif line.startswith("Edges: "):
                edges = int(line.split()[1])
            elif line.startswith("Indegree distribution:"):
                cur_deg_map = IN
            elif line.startswith("Outdegree distribution:"):
                cur_deg_map = OUT
            elif line.startswith("Degree distribution:"):
                cur_deg_map = IN_OUT
            elif line.startswith("deg="):
                sp = line[4:].split(": ")
                deg, cnt = int(sp[0]), int(sp[1])
                deg_maps[cur_deg_map][deg] = cnt
    print("%d nodes, %d edges" % (nodes, edges))
    print("%d nodes have 0 in-degree" % deg_maps[IN][0])
    print("%d nodes have 0 out-degree" % deg_maps[OUT][0])
    print("%d nodes have 0 degree" % deg_maps[IN_OUT][0])
    print("%d edges counted in src->tgt direction" % sum([k * v for k, v in deg_maps[OUT].items()]))
    print("%d edges counted in tgt<-src direction" % sum([k * v for k, v in deg_maps[IN].items()]))

if __name__ == "__main__":
    main()
