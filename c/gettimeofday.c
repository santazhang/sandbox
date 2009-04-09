#include <sys/time.h>
#include <time.h>
#include <stdio.h>

int main() {
  struct timeval tv;
  int i;
  for (i = 0; i < 1000000; i++) {
    gettimeofday(&tv, 0);
    printf("%d %d\n", tv.tv_sec, tv.tv_usec);
  }
  return 0;
}

