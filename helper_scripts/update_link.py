#!/usr/bin/python
# Update hard links of source code file among multiple projects

import os

def file_inode(f):
  if os.path.exists(f):
    st = os.lstat(f)
    return st.st_ino
  else:
    return -1

list_f = open("projects_list")
projects_list = map(str.strip, list_f.readlines())
if "" in projects_list:
  projects_list.remove("")
list_f.close()

def project_folder(f):
  global projects_list
  f = os.path.split(f)[0]
  for proj in projects_list:
    if f.startswith(proj):
      f = proj
  print "[project] %s" % f
  return f

def fix_modified_link(fpaths):
  print "fixing link..."
  print "files:", fpaths
  newest_f = None
  newest_mtime = None
  for f in fpaths:
#    print f, os.lstat(f).st_mtime
    mt = os.lstat(f).st_mtime
    if newest_mtime == None:
      newest_mtimt = mt
      newest_f = f
#    print "mt:", newest_mtime
    if newest_mtime < mt:
      newest_f = f
      newest_mtime = mt
    os.system('cd "%s" && git commit -a -m "auto commit by update_link.py"' % project_folder(f))
  print "newest file is %s" % newest_f
  print "all changes have been git-committed, you can safely revert if anything goes wrong"
  for f in fpaths:
    if f != newest_f:
      print "[del & re-link] %s" % f
      os.remove(f)
  print "re-linking..."
  link_file(*fpaths)

def link_file(*fpaths):
  inode = -1
  for f in fpaths:
    if os.path.exists(f):
      new_inode = file_inode(f)
      if inode == -1:
        inode = new_inode
      if inode != new_inode:
        print "warning! inode not equal for: ", fpaths
        choice = raw_input("automatically re-link to newest file?[y/N]")
        print
        if choice.startswith("y"):
          fix_modified_link(fpaths)
        else:
          return
      for other_f in fpaths:
        if other_f != f and os.path.exists(other_f) == False:
          other_f_folder = os.path.split(other_f)[0]
          if other_f_folder != '' and os.path.exists(other_f_folder) == False:
#            print "Making folder '%s'" % other_f_folder
            os.makedirs(other_f_folder)
          os.link(f, other_f)


def link_folder_walker(arg, dirname, fnames):
  folder, folders = arg
  for f in fnames:
    join_path = dirname + os.path.sep + f
    if (os.path.isdir(join_path)):
      continue
    child_path = join_path[len(folder):]
#    print "link_file:"
    files = []
    for flder in folders:
#      print flder + os.path.sep + child_path
      files += flder + os.path.sep + child_path,
    link_file(*files)
#    print


def link_folder(*folders):
  # make sure every folder exists
  for folder in folders:
    os.path.walk(folder, link_folder_walker, (folder, folders))

if __name__ == "__main__":
  def make_links(lnk_grp):
    if len(lnk_grp) == 0:
      return
    if os.path.isfile(lnk_grp[0]):
      print "link_file", lnk_grp
      link_file(*lnk_grp)
    elif os.path.isdir(lnk_grp[0]):
      print "link_folder", lnk_grp
      link_folder(*lnk_grp)

  f = open("links_list")
  link_group = []
  for line in f.readlines():
    line = line.strip()
    if line == "":
      make_links(link_group)
      link_group = []
    else:
      link_group += line,
  make_links(link_group)  
  f.close()

