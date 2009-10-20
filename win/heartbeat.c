#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#else

// for linux
#include <unistd.h>

#endif  // #ifdef WIN32

#define YES 1
#define NO 0

int is_integer_str(char* str) {

  // ignore sign
  if (str[0] == '+' || str[0] == '-') {
    str++;
  }

  if (str[0] == '\0') {
    // 0-length str
    return NO;
  }

  while (str[0] != '\0') {
    if (str[0] > '9' || str[0] < '0') {
      return NO;
    }
    str++;
  }

  return YES;
}


void print_usage() {
  fprintf(stderr, "usage:\n");
  fprintf(stderr, "heartbeat\n");
  fprintf(stderr, "heartbeat message\n");
  fprintf(stderr, "heartbeat interval_in_milliseconds message\n");
}


void my_sleep(int milli) {
#ifdef WIN32
  Sleep(milli);
#else
  // for linux
  usleep(milli * 1000);
#endif  // #ifdef WIN32
}


int main(int argc, char* argv[]) {

  char* message = "heartbeat";

  int interval = 1000;

  // process command line
  if (argc == 2) {
    message = argv[1];
  } else if (argc == 3) {
    if (is_integer_str(argv[1]) && (interval = atoi(argv[1])) >= 0) {
      message = argv[2];
    } else {
      fprintf(stderr, "Incorrect time format\n");
      print_usage();
      exit(1);
    }
  } else if (argc > 3) {
    fprintf(stderr, "Incorrect parameters\n");
    print_usage();
    exit(1);
  }

  // work with the heartbeating loop
  for (;;) {
    printf("%s\n", message);
    my_sleep(interval);
  }

  return 0;
}
