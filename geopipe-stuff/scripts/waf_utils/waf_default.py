# Default waf configuration
# Yang Zhang <y@yzhang.net>, 2015
# See LICENSE file for copyright notice

from __future__ import print_function
import os
import sys

from utils import LINUX, MACOSX, check_env
from waflib import Logs, Context
from waflib.Errors import ConfigurationError


def options(opt):
    g = opt.get_option_group("Build and installation options")
    default_build_variant = "debug" if check_env("DEBUG") else "release"
    g.add_option("--variant", help="Build variant (release/debug)",
                 action="store", default=default_build_variant, dest="variant")


def configure(conf):
    conf.env.append_value("CFLAGS", "-fPIC")
    conf.env.append_value("CXXFLAGS", "-fPIC")
    conf.env.append_value("LINKFLAGS", "-fPIC")

    conf.env.append_value("CFLAGS", "-std=c99")

    # C++11 support
    if MACOSX:
        conf.env.append_value("CXXFLAGS", "-stdlib=libc++")
        conf.env.append_value("LINKFLAGS", "-stdlib=libc++")
    conf.env.append_value("CXXFLAGS", "-std=c++0x")

    # build variant
    conf.env.append_value("CFLAGS", ["-Wall", "-pthread", "-ggdb"])
    conf.env.append_value("CXXFLAGS", ["-Wall", "-pthread", "-ggdb"])
    if conf.options.variant == "debug":
        Logs.pprint("PINK", "DEBUG build enabled")
    elif conf.options.variant == "release":
        conf.env.append_value("CFLAGS", ["-O3", "-fno-omit-frame-pointer"])
        conf.env.append_value("CXXFLAGS", ["-O3", "-fno-omit-frame-pointer"])
        conf.env.append_value("DEFINES", "NDEBUG")
    else:
        raise ConfigurationError("Unknown build variant '%s'"
                                 % conf.options.variant)

    # warnings
    flags = """
    -Wextra -pedantic -Wformat=2 -Wunused-parameter -Wshadow
    -Wwrite-strings -Wredundant-decls -Wmissing-include-dirs
    -Wno-format-nonliteral -fstrict-aliasing
    """
    if MACOSX:
        flags += """
        -Wno-gnu -Wstrict-prototypes -Wold-style-definition
        -Wno-nested-anon-types -Wnested-externs -Wthread-safety
        """
    conf.env.append_value("CFLAGS", flags.split())
    conf.env.append_value("CXXFLAGS", flags.split())
    conf.env.append_value("CXXFLAGS", "-Wold-style-cast")

    # extra flags
    conf.env.append_value("DEFINES", 'WORKSPACE="%s/"' % Context.top_dir)

    # compiler info
    with os.popen('%s --version | head -n 1' % conf.env.CXX[0]) as f:
        info = f.read().strip()
        conf.env.append_value('CXXFLAGS', '-DCOMPILER_INFO="%s"' % info)
    with os.popen('%s --version | head -n 1' % conf.env.CC[0]) as f:
        info = f.read().strip()
        conf.env.append_value('CFLAGS', '-DCOMPILER_INFO="%s"' % info)

    # third_party include path
    p = os.path.abspath("deps/_install/include")
    conf.env.append_value("CFLAGS", ["-isystem", p])
    conf.env.append_value("CXXFLAGS", ["-isystem", p])
