# Utility code for wscript
# Yang Zhang <y@yzhang.net>, 2015
# See LICENSE file for copyright notice

from __future__ import print_function
import os
import sys

import re
import string
import subprocess
import time


if sys.platform == "darwin":
    MACOSX = True
    LINUX = False
elif sys.platform.startswith('linux'):
    MACOSX = False
    LINUX = True
else:
    raise NotImplementedError("Unsupported OS: %s" % sys.platform)


_p = os.getcwd()
while True:
    if os.path.isfile(os.path.join(_p, "WORKSPACE")):
        WORKSPACE = _p
        break
    if _p == os.path.dirname(_p):
        break
    _p = os.path.dirname(_p)


# Based on waflib.Utils.run_once, support variable number of function arguments
def cache_result(fun):
    cache = {}

    def wrap(*k):
        if k not in cache:
            cache[k] = fun(*k)
        return cache[k]

    wrap.__cache__ = cache
    return wrap


class termcolor(object):
    BLACK = 0
    RED = 1
    GREEN = 2
    BROWN = 3
    BLUE = 4
    MAGENTA = 5
    CYAN = 6
    LIGHT_GRAY = 7
    DARK_GRAY = 8
    LIGHT_RED = 9
    LIGHT_GREEN = 10
    YELLOW = 11
    LIGHT_BLUE = 12
    LIGHT_MAGENTA = 13
    LIGHT_CYAN = 14
    WHITE = 15


def set_termcolor(file, color):
    try:
        if not file.isatty():
            return
    except:
        return
    if color is None:
        file.write("\x1B[0m")
        file.flush()
    else:
        file.write("\x1B[%d;%dm" % (color // 8, 30 + color % 8))


def red(str):
    return "\x1B[0;31m%s\x1B[0m" % str


def green(str):
    return "\x1B[0;32m%s\x1B[0m" % str


def brown(str):
    return "\x1B[0;33m%s\x1B[0m" % str


def blue(str):
    return "\x1B[0;34m%s\x1B[0m" % str


def puts(*objs, **opts):
    file = opts.get("file", sys.stdout)
    sep = opts.get("sep", " ")
    end = opts.get("end", "\n")
    color = opts.get("color", None)

    if color is not None:
        set_termcolor(file, color)

    file.write(sep.join(map(str, objs)) + end)
    file.flush()

    if color is not None:
        set_termcolor(file, None)


def mkdir_p(dpath):
    if not os.path.exists(dpath):
        os.makedirs(dpath)


def year_now():
    return time.localtime().tm_year


def run_cmd(cmd, check=True, cwd=None, shell=True, echo=True):
    if echo:
        if cwd is not None:
            end = " (in %s)\n" % cwd
        else:
            end = "\n"
        if type(cmd) == list:
            puts(" ".join(cmd), end=end, color=termcolor.BLUE)
        else:
            puts(cmd, end=end, color=termcolor.BLUE)
    if check:
        return subprocess.check_call(cmd, cwd=cwd, shell=shell)
    else:
        return subprocess.call(cmd, cwd=cwd, shell=shell)


def iter_files_and_dirs(iter_root):
    if type(iter_root) is str:
        iter_root = iter_root.split()
    for r in iter_root:
        if os.path.isdir(r):
            for root, dnames, fnames in os.walk(r):
                dnames.sort()
                fnames.sort()
                for dn in dnames:
                    dpath = os.path.join(root, dn)
                    yield dpath
                for fn in fnames:
                    fpath = os.path.join(root, fn)
                    yield fpath
        elif os.path.exists(r):
            yield r
        else:
            puts("Path '%s' not found!" % r, color=termcolor.RED)


def iter_files(iter_root):
    for e in iter_files_and_dirs(iter_root):
        if os.path.isfile(e):
            yield e


def iter_lines_in_file(fpath):
    with open(fpath) as f:
        lineno = 0
        for l in f:
            lineno += 1
            yield l, lineno


# #include "..." and <...>
def iter_cpp_includes(fpath):
    for l, lineno in iter_lines_in_file(fpath):
        l = l.strip()
        if l.startswith("#include"):
            sp = re.split("[\"<>]+", l)
            if len(sp) < 2:
                continue
            inc_fpath = sp[1]
            yield inc_fpath


# only #include "..."
def iter_cpp_local_includes(fpath):
    fpath = patch_path_for_experimental_code(fpath)
    for l, lineno in iter_lines_in_file(fpath):
        l = l.strip()
        if l.startswith("#include") and "<" not in l:
            sp = re.split("\"+", l)
            inc_fpath = sp[1]
            yield inc_fpath


def patch_path_for_experimental_code(fpath):
    if fpath.startswith("experimental"):
        return fpath
    experimental_fpath = os.path.join("experimental", fpath)
    assert not (os.path.exists(fpath) and os.path.exists(experimental_fpath))
    if os.path.exists(fpath):
        return fpath
    elif os.path.exists(experimental_fpath):
        return experimental_fpath
    else:
        puts("%s: file not found" % red(fpath))
        return fpath


# http://code.activestate.com/recipes/173220/
def is_text_file(fpath):
    with open(fpath) as f:
        s = f.read(4 * 1024)
    text_characters = "".join(list(map(chr, list(range(32, 127))))
                              + list("\n\r\t\b"))
    _null_trans = string.maketrans("", "")
    if not s:
        # Empty files are considered text
        return True
    if "\0" in s:
        # Files with null bytes are likely binary
        return False
    # Get the non-text characters (maps a character to itself then
    # use the 'remove' option to get rid of the text characters.)
    t = s.translate(_null_trans, text_characters)
    # If more than 30% non-text characters, then
    # this is considered a binary file
    if float(len(t)) / len(s) > 0.3:
        return False
    return True


def regexp_match_any_of(s, patterns):
    for p in patterns:
        if re.search(p, s):
            return p


def check_env(name, default=False):
    v = os.getenv(name)
    if v is None:
        return default
    elif v.lower() in ["1", "y", "yes", "t", "true"]:
        return True
    elif v.lower() in ["0", "n", "no", "f", "false"]:
        return False
    else:
        raise ValueError("%s=%s not supported! Choose from {0, 1}" % (name, v))


def split_list(l):
    return l.split() if isinstance(l, str) else list(l)


def read_file(path):
    with open(path) as f:
        return f.read()


def git_dirty_files(include_untracked=False):
    files = set()
    with os.popen("git status --porcelain") as p:
        for l in p.readlines():
            if not include_untracked and l.startswith("??"):
                continue
            for sp in l[3:].strip().split():
                if sp == "->":
                    continue
                elif os.path.exists(sp):
                    files.add(sp)
    return list(sorted(files))


def git_ls_files(seed_paths):
    files = set()
    if len(seed_paths) == 0:
        seed_paths = ["."]
    for seed_path in seed_paths:
        with os.popen("git ls-files --full-name '%s'" % seed_path) as p:
            for l in p.readlines():
                l = l.strip()
                files.add(l)
    return list(sorted(files))


# Tarjan SCC algorithm
class SCC(object):
    def __init__(self, graph):
        self.graph = graph
        self.result = []
        self.index = {}
        self.lowlink = {}
        self.next_index = 0
        self.stack = []

    def scc(self, v):
        self.index[v] = self.next_index
        self.lowlink[v] = self.next_index
        self.next_index += 1
        self.stack += v,
        for w in self.graph[v]:
            if w not in self.index:
                self.scc(w)
                self.lowlink[v] = min(self.lowlink[v], self.lowlink[w])
            elif w in self.stack:
                self.lowlink[v] = min(self.lowlink[v], self.index[w])
        if self.lowlink[v] == self.index[v]:
            new_scc = []
            while True:
                w = self.stack.pop()
                new_scc += w,
                if w == v:
                    break
            self.result += new_scc,

    def report(self):
        for v in self.graph:
            if v not in self.index:
                self.scc(v)
        return self.result
