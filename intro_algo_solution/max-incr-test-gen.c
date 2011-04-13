#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int solve(int* a, int len) {
  int m, i, j;
  int *s = (int *) malloc(len * sizeof(int));
  for (i = 0; i < len; i++) {
    s[i] = 1;
  }
  for (i = 1; i < len; i++) {
    for (j = i - 1; j >= 0; j--) {
      if (a[i] > a[j] && s[i] < s[j] + 1) {
        s[i] = s[j] + 1;
      }
    }
  }
  m = s[0];
  for (i = 1; i < len; i++) {
    if (m < s[i]) {
      m = s[i];
    }
  }
  free(s);
  return m;
}

static int* gen(int n) {
  int* arr = (int *) malloc(n * sizeof(int));
  int i;
  srand(time(NULL));
  for (i = 0; i < n; i++) {
    arr[i] = rand() % 10000;
    if (rand() % 2) {
      arr[i] = -arr[i];
    }
  }
  return arr;
}

int main(int argc, char* argv[]) {
  int n, i;
  int *arr;
  if (argc < 2) {
    printf("test case generator for problem 15.4-5, Introduction to Algorithms\n");
    printf("usage: max-incr-test-gen <N>\n");
    printf("<N> is the size of output array.\n");
    exit(0);
  }
  sscanf(argv[1], "%d", &n);
  if (n > 1000) {
    n = 1000;
  }
  printf("%d\n", n);
  arr = gen(n);
  for (i = 0; i < n; i++) {
    printf("%d ", arr[i]);
  }
  printf("\n");
  printf("%d\n", solve(arr, n));
  free(arr);
  return 0;
}

