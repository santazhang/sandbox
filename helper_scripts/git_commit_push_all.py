#!/usr/bin/python

# push all my craps to their remote repositories

import os

list_f = open("projects_list")
repo_list = map(str.strip, list_f.readlines())
repo_list.remove("")
list_f.close()

for repo in repo_list:
  pipe = os.popen("cd %s && git remote" % repo)
  remotes = map(str.strip, pipe.readlines())
  pipe.close()
  for remote in remotes:
    cmd = 'cd %s && git commit -a -m "auto commit" && git push %s --all' % (repo, remote)
    print cmd
    os.system(cmd)

