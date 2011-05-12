#include <stdio.h>
#include "foo.h"

void foo::output() {
  printf("This is called\n");
}

void foo_for_c() {
  foo bar;
  bar.output();
}
