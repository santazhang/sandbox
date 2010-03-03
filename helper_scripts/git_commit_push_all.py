#!/usr/bin/python

# push all my craps to their remote repositories

import os

list_f = open("projects_list")
repo_list = map(str.strip, list_f.readlines())
if "" in repo_list:
  repo_list.remove("")
list_f.close()

def my_exec(cmd):
  print "[cmd] %s" % cmd
  os.system(cmd)

for repo in repo_list:
  pipe = os.popen("cd %s && git remote" % repo)
  remotes = map(str.strip, pipe.readlines())
  pipe.close()
  my_exec('cd "%s" && git commit -a -m "auto commit by commit-push-all.py"' % repo)
  for remote in remotes:
    my_exec('cd "%s" && git push %s --all' % (repo, remote))
  print
