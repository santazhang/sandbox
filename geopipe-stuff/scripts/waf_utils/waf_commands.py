# Extra commands supported in wscript
# Yang Zhang <y@yzhang.net>, 2015
# See LICENSE file for copyright notice

import os
from waflib import Build, Logs, Utils

class MakefileGen(Build.ListContext):
    """generate makefile"""

    cmd = "mkgen"

    def execute(self):
        orig_pprint = Logs.pprint
        Logs.pprint = Utils.nada
        super(MakefileGen, self).execute()
        Logs.pprint = orig_pprint

        build_targets = sorted(self.task_gen_cache_names.keys())
        with open(os.path.join(self.out_dir, "targets.mk"), "w") as f:
            f.write("# Generated by `./waf mkgen`. DO NOT EDIT!\n\n")
            f.write(".PHONY: %s\n\n" % " ".join(build_targets))
            f.write("targets:\n\t@echo %s\n\n" % " ".join(build_targets))
            for target in build_targets:
                f.write("%s:\n" % target)
                f.write("\t$(WAF_PRELUDE)\n")
                f.write("\t$(WAF) --targets=%s\n\n" % target)