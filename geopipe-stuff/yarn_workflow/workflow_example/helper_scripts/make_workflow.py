#!/usr/bin/env python

N_STEPS = 10

for i in range(N_STEPS):
    with open("../step-%d.sh" % i, "w") as f:
        f.write("echo STEP %d\n" % i)

with open("../WORKFLOW", "w") as f:
    f.write("# Makefile-ish dependency\n")
    for i in range(N_STEPS - 1):
        f.write("step-%d.sh : step-%d.sh\n" % (i + 1, i))
