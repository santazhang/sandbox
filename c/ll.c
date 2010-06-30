#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
  int i;
  char cmd[8192];
  strcpy(cmd, "ls -lah");
  for (i = 1; i < argc; i++) {
    strcat(cmd, " ");
    strcat(cmd, argv[i]);
  }
  system(cmd);
  return 0;
}
