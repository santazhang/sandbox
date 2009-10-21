#include <stdio.h>
#include <stdlib.h>

int mu(int n) {
  if (n == 1) {
    return 1;
  } else {
    int p;
    int k = 0;

    if (n % 2 == 0) {
      n /= 2;
      //printf("%d ", 2);
      if (n % 2 == 0) {
        return 0;
      }
      k += 1;
    }
    for (p = 3; p <= n; p += 2) {
      if (n % p == 0) {
        n /= p;
        //printf("%d ", p);
        if (n % p == 0) {
          return 0;
        }
        k += 1;
      }
    }
    if (k % 2 == 0)
      return 1;
    else
      return -1;
  }
}

#define N_CACHE 1000001
int cache[N_CACHE];

int cached_mu(int n) {
  if (cache[n] == -2) {
    cache[n] = mu(n);
  }
  return cache[n];
}


int solution(int n) {
  int sum = -1;
  int i;
  for (i = 1; i <= n; i++) {
    sum += cached_mu(i) * (n / i) * (n / i);
  }
  sum /= 2;
  return sum;
}

int main() {
  int n;
  for (n = 0; n < N_CACHE; n++) {
    cache[n] = -2;
  }
  while(scanf("%d", &n) == 1) {
    if (n == 0)
      break;
    printf("%d\n", solution(n));
  }
  return 0;
}

