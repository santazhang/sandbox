#!/usr/bin/env python
# Show all the depended packages, according to "apt-cache depends" result.

import os

print "This script uses output of 'apt-cache depends' to determine all depended packages."

def apt_depends_helper(pkg_name):
  f = os.popen("apt-cache depends %s" % pkg_name)
  depends = []
  for line in f.readlines():
    if line.startswith("  "):
      if line.startswith("  Depends:"):
        pkg = line.split()[1]
        if not pkg.startswith("<"):
          depends.append(pkg)
      elif line.startswith("    "):
        pkg = line.strip()
        if not pkg.startswith("<"):
          depends.append(pkg)
      else:
        break
  f.close()
  print "%s:" % pkg_name,
  for pkg in depends:
    print pkg,
  print
  return depends

def apt_depends(deb_list):
  open_list = deb_list
  closed_list = []
  while len(open_list) > 0:
    pkg = open_list.pop()
    closed_list.append(pkg)
    for depend_pkg in apt_depends_helper(pkg):
      if depend_pkg not in closed_list and depend_pkg not in open_list:
        open_list.append(depend_pkg)
  return closed_list


pkgs_input = raw_input("input packages in a line, separate with space:\n")
pkgs = pkgs_input.split()
pkgs_depend = apt_depends(pkgs)
pkgs_depend.sort()
print pkgs_depend
down_dir = raw_input("Download folder: ")
partial_folder = down_dir + os.path.sep + "partial"
if not os.path.exists(partial_folder):
  os.makedirs(partial_folder)
for pkg in pkgs_depend:
  cmd = "sudo apt-get install --reinstall -d -y -o Dir::Cache::Archives=%s %s" % (down_dir, pkg)
  print cmd
  os.system(cmd)
