#!/usr/bin/python

# push all my craps to their remote repositories

import os

repo_list = [
  "dummy_vm",
  "jos/lab",
  "liquid",
  "liquid_admin",
  "musql",
  "nova",
  "paper_impl",
  "port_mapper",
  "rainmeter_adapter",
  "sandbox",
  "simpledb/lab",
  "vnc_proxy",
  "xdk",
  "yfs/lab"
]

for repo in repo_list:
  pipe = os.popen("cd %s && git remote" % repo)
  remotes = map(str.strip, pipe.readlines())
  pipe.close()
  for remote in remotes:
    cmd = 'cd %s && git commit -a -m "auto commit" && git push %s --all' % (repo, remote)
    print cmd
    os.system(cmd)

