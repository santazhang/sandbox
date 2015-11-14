#!/usr/bin/env python

from __future__ import print_function

import os
import time
import calendar
import re
import subprocess
import socket

REV_SINCE = "2015-10-19 12:00:00"
REV_SINCE_TM = calendar.timegm(time.strptime(REV_SINCE, "%Y-%m-%d %H:%M:%S"))

if "get-devstuff.sh" not in os.listdir("."):
    print("Run this script with get-devstuff.sh in current dir")
    exit(1)

with open("get-devstuff.sh") as f:
    base_build_script = f.read()

os.system("mkdir -p find_buildable_rev")
os.chdir("find_buildable_rev")

time_fmt = "%a %b %d %H:%M:%S %Y"

def find_revs(github_repo, repo_name):
    os.system("git clone %s > /dev/null 2>&1" % github_repo)
    os.chdir(repo_name)
    os.system("git checkout master > /dev/null 2>&1")
    os.system("git pull > /dev/null 2>&1")
    revs = [] # elements: (time, rev)
    with os.popen("git log --pretty='%%H/%%ad' --since='%s'" % REV_SINCE) as p:
        for l in p.readlines():
            sp = l.strip()[:-6].split("/")
            r = sp[0]
            t = calendar.timegm(time.strptime(sp[1], time_fmt))
            revs += (t, r),
    n_commits = len(revs)
    print("%d commits to %s since %s" % (n_commits, repo_name, REV_SINCE))
    if n_commits == 0:
        with os.popen("git log --pretty='%H/%ad' | head -n 1") as p:
            for l in p.readlines():
                sp = l.strip()[:-6].split("/")
                r = sp[0]
                t = calendar.timegm(time.strptime(sp[1], time_fmt))
                revs += (t, r),
        print("using latest rev for %s: %s" % (repo_name, time.strftime(time_fmt, time.gmtime(revs[0][0]))))
    os.chdir("..")
    return list(sorted(revs, reverse=True))

folly_revs = find_revs("https://github.com/facebook/folly.git", "folly")
wangle_revs = find_revs("https://github.com/facebook/wangle.git", "wangle")
proxygen_revs = find_revs("https://github.com/facebook/proxygen.git", "proxygen")
fbthrift_revs = find_revs("https://github.com/facebook/fbthrift.git", "fbthrift")

commit_times = set()
for revs in [folly_revs, wangle_revs, proxygen_revs, fbthrift_revs]:
    for r in revs:
        if r[0] >= REV_SINCE_TM:
            commit_times.add(r[0])
commit_times = list(sorted(commit_times))
print("%d commit points to check" % len(commit_times))

def find_commit(tm, revs):
    # revs is sorted in reverse order
    for r in reversed(revs):
        if r[0] >= tm:
            return r[1]
    return revs[-1][1]

for t in commit_times:
    print()
    print("=============")
    print("checking commit time: %s" % time.strftime(time_fmt, time.gmtime(t)))
    folly_commit = find_commit(t, folly_revs)
    print("folly commit: %s" % folly_commit)
    wangle_commit = find_commit(t, wangle_revs)
    print("wangle commit: %s" % wangle_commit)
    proxygen_commit = find_commit(t, proxygen_revs)
    print("proxygen commit: %s" % proxygen_commit)
    fbthrift_commit = find_commit(t, fbthrift_revs)
    print("fbthrift commit: %s" % fbthrift_commit)
    print("=============")

    build_script = ""
    for l in base_build_script.split("\n"):
        if re.match("^get_[a-zA-Z0-9]+$", l):
            if l not in ["get_folly", "get_wangle", "get_proxygen", "get_fbthrift"]:
                continue
        if l.startswith("FOLLY_VERSION="):
            l = "FOLLY_VERSION=\"%s\"\necho FOLLY: $FOLLY_VERSION" % folly_commit
        elif l.startswith("WANGLE_VERSION="):
            l = "WANGLE_VERSION=\"%s\"\necho WANGLE: $WANGLE_VERSION" % wangle_commit
        elif l.startswith("PROXYGEN_VERSION="):
            l = "PROXYGEN_VERSION=\"%s\"\necho PROXYGEN: $PROXYGEN_VERSION" % proxygen_commit
        elif l.startswith("FBTHRIFT_VERSION="):
            l = "FBTHRIFT_VERSION=\"%s\"\necho FBTHRIFT: $FBTHRIFT_VERSION" % fbthrift_commit
        build_script += l + "\n"
    with open("patched-get-devstuff.sh", "w") as f:
        f.write(build_script)
    os.system("chmod a+x patched-get-devstuff.sh")
    print("building...")
    os.system("rm -rf devstuff")
    os.system("./patched-get-devstuff.sh 2>&1 | tee build_rev_date_%d.log" % t)

    build_ok = True
    build_ok = build_ok and os.path.exists("devstuff/src/folly/VERSION")
    build_ok = build_ok and os.path.exists("devstuff/src/wangle/VERSION")
    build_ok = build_ok and os.path.exists("devstuff/src/proxygen/VERSION")
    # build_ok = build_ok and os.path.exists("devstuff/src/fbthrift/VERSION")

    if build_ok:
        print("=============")
        print("build: OK")
        print("=============")
        print()
        outcome = "fbstuff build OK: "
    else:
        print("=============")
        print("build: FAIL")
        print("=============")
        print()
        outcome = "fbstuff build FAIL: "
    outcome += "rev_date=%s, host=%s, folly=%s, wangle=%s, proxygen=%s, fbthrfit=%s" % (
        time.strftime(time_fmt, time.gmtime(t)), socket.gethostname(),
        folly_commit[0:7], wangle_commit[0:7], proxygen_commit[0:7], fbthrift_commit[0:7])
    with open("build_outcome.log", "a") as f:
        f.write(outcome + "\n")
    if build_ok:
        os.system("pingsanta.py '%s'" % outcome)
    time.sleep(1)
