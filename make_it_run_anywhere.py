#!/usr/bin/env python

from __future__ import print_function

import os
import sys
import subprocess
import shutil
import stat


def copy_file(src, dst):
    print("Copy: %s -> %s" % (src, dst))
    shutil.copyfile(src, dst)


def mkdir_p(p):
    if not os.path.exists(p):
        os.makedirs(p)


def chmod_ax(fpath):
    st = os.stat(fpath)
    os.chmod(fpath, st.st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)


def panic(msg):
    print(msg)
    exit(1)


if not sys.platform.startswith("linux"):
    panic("  *** This script only works for linux")

if len(sys.argv) < 2:
    panic("Usage: %s <binary> [output-dir]" % sys.argv[0])
else:
    bin_fn = sys.argv[1]
    if len(sys.argv) >= 3:
        pkg_out = sys.argv[2]
    else:
        pkg_out = bin_fn + ".run_anywhere"

print("Make portable: %s => %s" % (bin_fn, pkg_out))

if not os.path.isfile(bin_fn):
    panic("   *** Not a file: %s" % bin_fn)

p = subprocess.Popen("ldd '%s'" % bin_fn, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
ret = p.wait()
stdout_data, stderr_data = p.communicate()
if ret != 0:
    panic("  *** Failed to analyse binary dependency! Is %s a binary program?\n\n( *** stdout *** )\n%s\n( *** stderr *** )\n%s" % (bin_fn, stdout_data, stderr_data))


so_missing = set()

def parse_dependency_helper(ldd_output, known_deps):
    new_deps = set()
    for l in ldd_output.split("\n"):
        l = l.strip()
        if l == "":
            continue
        if "=>" in l:
            sp = l.split("=>")
            so_name = sp[0].strip()
            if l.endswith("not found"):
                so_missing.add(so_name)
                continue
            so_fn = sp[1].strip().split()[0]
            if so_fn.startswith("(0x"):
                continue  # linux-vdso
            if so_fn not in known_deps:
                new_deps.add(so_fn)
        else:
            # ld-so
            sp = l.split()
            ld_so = sp[0]
            if ld_so not in known_deps:
                new_deps.add(ld_so)

    all_deps = set(known_deps).union(new_deps)

    for fn in new_deps:
        p = subprocess.Popen("ldd '%s'" % fn, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        ret = p.wait()
        stdout_data, stderr_data = p.communicate()
        if ret == 0 and stdout_data.strip() != "statically linked":
            sub_deps = parse_dependency_helper(stdout_data, all_deps)
            for x in sub_deps:
                all_deps.add(x)

    return all_deps


def parse_dependency(ldd_output):
    return parse_dependency_helper(ldd_output, [])


dep_files = parse_dependency(stdout_data)
payload_dir = os.path.join(pkg_out, ".run_anywhere_payload")
mkdir_p(payload_dir)

ld_linux_so = None

for fn in dep_files:
    fn_base = os.path.basename(fn)
    if fn_base.startswith("ld-linux-"):
        ld_linux_so = fn_base
    new_fn = os.path.join(payload_dir, fn_base)
    copy_file(fn, new_fn)

bin_fn_base = os.path.basename(bin_fn)
bin_fn_real = os.path.join(payload_dir, bin_fn_base + ".run_anywhere_real_binary")
bin_fn_wrapper = os.path.join(pkg_out, bin_fn_base)
copy_file(bin_fn, bin_fn_real)
with open(bin_fn_wrapper, "w") as f:
    f.write("#!/bin/bash\n")
    relpath = os.path.join(".run_anywhere_payload", bin_fn_base + ".run_anywhere_real_binary")
    f.write("""
SOURCE="${BASH_SOURCE[0]}"
# Resolve $SOURCE until the file is no longer a symlink
while [ -h "$SOURCE" ]; do
    DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
    SOURCE="$(readlink "$SOURCE")"
    # If $SOURCE was a relative symlink, we need to resolve it relative to the
    # path where the symlink file was located
    [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

# export LD_DEBUG=files,statistics

LD_LIBRARY_PATH=$DIR/.run_anywhere_payload:$LD_LIBRARY_PATH $DIR/.run_anywhere_payload/%s --inhibit-cache --inhibit-rpath $DIR/%s $DIR/%s "$@"
    """.strip() % (ld_linux_so, relpath, relpath) + "\n")

chmod_ax(os.path.join(payload_dir, ld_linux_so))
chmod_ax(bin_fn_wrapper)

if len(so_missing) > 0:
    print("\n  *** Missing: %s\n" % (" ".join(sorted(so_missing))))
