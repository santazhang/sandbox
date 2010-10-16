/*************************************************************************
* Author: Santa Zhang
* Created Time: 2009-11-26 20:31:12
* File Name: hw1.c
* Description: 
 ************************************************************************/

#include <stdio.h>

#define NUM_STEPS 100000

int main() {
  int i;
  double step = 1.0 / (double) NUM_STEPS;
  double sum = 0.0;
  double x, pi;

  #pragma omp parallel for private(x) reduction(+:sum)
  for (i = 0; i < NUM_STEPS; i++) {
    x = (i + 0.5) * step;
    sum += 4.0 / (1.0 + x * x);
  }
  pi = step * sum;
  printf("pi = %lf\n", pi);
  return 0;
}

