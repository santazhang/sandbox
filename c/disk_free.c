/*************************************************************************
* Author: Santa Zhang
* Created Time: 2010-01-13 01:35:02
* File Name: disk_free.c
* Description: Check disk free space
 ************************************************************************/

#include <sys/statvfs.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
  struct statvfs st;
  char* fn;
  if (argc == 1) {
    fn = argv[0];
  } else if (argc >= 2) {
    fn = argv[1];
  }
  if (statvfs(fn, &st) == 0) {
    printf("disk free @ %s is %llu, %.2lfG\n", fn, (long long unsigned) st.f_bavail * st.f_bsize, ((long long unsigned)st.f_bavail * st.f_bsize) / 1024.0 / 1024.0 / 1024.0);
    printf("== info ==\n");
    printf("file system block size:       %lu\n", st.f_bsize);
    printf("fragment size:                %lu\n", st.f_frsize);
    printf("size of fs in f_frsize units: %lu\n", st.f_blocks);
    printf("free blocks:                  %lu\n", st.f_bfree);
    printf("free blocks for non-root:     %lu\n", st.f_bavail);
    printf("inodes:                       %lu\n", st.f_files);
    printf("free inodes:                  %lu\n", st.f_favail);
    printf("file system id:               %lu\n", st.f_fsid);
    printf("mount flags:                  %lx\n", st.f_flag);
    printf("maximum filename length:      %lu\n", st.f_namemax);
  }
  return 0;
}
