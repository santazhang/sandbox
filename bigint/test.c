#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __APPLE__
#include <malloc.h>
#endif

#include "bigint.h"

void do_test(char* a, char* op, char* b) {
  bigint va;
  bigint vb;
  char* buf = NULL;
  bigint_init(&va);
  bigint_init(&vb);

  bigint_from_string(&va, a);
  bigint_from_string(&vb, b);

  if (strcmp(op, "+") == 0) {
    bigint_add_by(&va, &vb);
  } else if (strcmp(op, "-") == 0) {
    bigint_sub_by(&va, &vb);
  } else if (strcmp(op, "times") == 0) {
    bigint_mul_by(&va, &vb);
  } else if (strcmp(op, "pow") == 0) {
    int v;
    bigint_to_int(&vb, &v);
    bigint_pow_by_int(&va, v);
  } else {
    printf("err: op not known: %s\n", op);
  }

  buf = (char *) malloc(bigint_string_length(&va) + 10);
  bigint_to_string(&va, buf);
  printf("va = %s\n", buf);

  bigint_release(&va);
  bigint_release(&vb);
  free(buf);
}

int main(int argc, char* argv[]) {
  if (argc != 4) {
    printf("usage: test a op b\n");
    return 0;
  }
  do_test(argv[1], argv[2], argv[3]);
  return 0;
}

