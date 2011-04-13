#include <stdio.h>
#include <stdlib.h>

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

int main(int argc, char* argv[]) {
  int i, len;
  int *a;
  scanf("%d", &len);
  a = (int *) malloc(len * sizeof(int));
  for (i = 0; i < len; i++) {
    scanf("%d", &a[i]);
  }
  printf("%d\n", solve(a, len));
  free(a);
  return 0;
}

