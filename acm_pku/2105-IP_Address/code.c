#include <stdio.h>

int main() {
  char ip[33];
  int n, i, j, k;
  int seg;
  scanf("%d\n", &n);
  for (i = 0; i < n; i++) {
    scanf("%s\n", ip);
    for (j = 0; j < 4; j++) {
      seg = 0;
      if (j != 0) {
        printf(".");
      }
      for (k = j * 8; k < (j + 1) * 8; k++) {
        seg *= 2;
        seg += ip[k] - '0';
      }
      printf("%d", seg);
    }
    printf("\n");
  }
  return 0;
}
