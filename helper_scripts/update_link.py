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
  link_file("liquid/Rakefile", "xdk/Rakefile", "vnc_proxy/Rakefile", "dummy_vm/Rakefile", "port_mapper/Rakefile", "nova/tools/Rakefile")

  link_file("sandbox/bigint/bigint.c", "xdk/3rdparty/bigint/bigint.c")
  link_file("sandbox/bigint/bigint.h", "xdk/3rdparty/bigint/bigint.h")

  link_file("liquid/test/test_path_normalize.c", "xdk/test/test_path_normalize.c")
  link_file("liquid/test/test_md5sum.c", "xdk/test/test_md5sum.c")
  link_file("liquid/test/test_sha1.c", "xdk/test/test_sha1.c")
  link_file("liquid/test/test_xbigint.c", "xdk/test/test_xbigint.c")
  link_file("liquid/test/test_xhash.c", "xdk/test/test_xhash.c")

  link_folder("xdk/src", "vnc_proxy/xdk", "liquid/xdk", "dummy_vm/xdk", "port_mapper/xdk", "nova/tools/xdk")
  link_folder("dummy_vm/src", "nova/tools/dummy_vm")
  link_folder("port_mapper/src", "nova/tools/port_mapper")
  link_folder("vnc_proxy/src", "nova/tools/vnc_proxy")
  link_folder("xdk/3rdparty/bigint", "liquid/3rdparty/bigint", "vnc_proxy/3rdparty/bigint", "dummy_vm/3rdparty/bigint", "port_mapper/3rdparty/bigint", "nova/tools/3rdparty/bigint")
  link_folder("xdk/3rdparty/crypto", "liquid/3rdparty/crypto", "vnc_proxy/3rdparty/crypto", "dummy_vm/3rdparty/crypto", "port_mapper/3rdparty/crypto", "nova/tools/3rdparty/crypto")
  link_folder("vnc_proxy/3rdparty/vnc_auth", "nova/tools/3rdparty/vnc_auth")

