/*************************************************************************
* Author: Santa Zhang
* Created Time: 2009-11-26 20:31:12
* File Name: hw1.c
* Description: 
 ************************************************************************/

#include <stdio.h>
#include "mpi.h"

#define NUM_STEPS 100000

int main(int argc, char *argv[]) {
  int rank;
  int size;

  int i;
  double step = 1.0 / (double) NUM_STEPS;
  double sum = 0.0;
  double x, partial_pi, pi;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  for (i = rank; i < NUM_STEPS; i += size) {
    x = (i + 0.5) * step;
    sum += 4.0 / (1.0 + x * x);
  }
  partial_pi = step * sum;

  MPI_Reduce(&partial_pi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  if (rank == 0) {
    printf("pi = %lf\n", pi);
  }

  MPI_Finalize();
  return 0;
}

