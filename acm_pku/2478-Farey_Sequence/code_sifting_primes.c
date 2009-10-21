#include <stdio.h>

#define N_LIM 1000005
int mu_k_tab[N_LIM];

void build_mu_tab() {
  int i;
  for (i = 0; i < N_LIM; i++) {
    mu_k_tab[i] = -2;
  }
  mu_k_tab[1] = 0;
  
  for (i = 2; i < N_LIM; i++) {
    int k;
    int j;
    
    if (mu_k_tab[i] != -2)
      continue;
    
    mu_k_tab[i] = 1;
    for (k = 2; k * i < N_LIM; k++) {
      j = k * i;
      if (mu_k_tab[j] == -2) {
        mu_k_tab[j] = 1;
      } else if (mu_k_tab[j] > 0) {
        mu_k_tab[j] += 1;
      }
      if (k % i == 0) {
        mu_k_tab[j] = -1;
      }
    }
  }
}


int mu(int n) {
  if (mu_k_tab[n] < 0)
    return 0;
  if (mu_k_tab[n] % 2 == 0)
    return 1;
  else
    return -1;
}


long long solution(int n) {
  long long sum = -1;
  int i;
  for (i = 1; i <= n; i++) {
    sum += mu(i) * (n / i) * (n / i);
  }
  sum /= 2;
  return sum;
}


int main() {
  int n;
  build_mu_tab();
  while(1) {
    scanf("%d", &n);
    if (n == 0)
      break;
    printf("%lld\n", solution(n));
  }
  return 0;
}

