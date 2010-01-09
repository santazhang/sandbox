#include <stdio.h>


void print() {
  
#ifdef WIN32
  printf("Platform: WIN32\n");
#endif
  
#ifdef WIN64
  printf("Platform: WIN64\n");
#endif
  
#ifdef WINDOWS
  printf("Platform: WINDOWS\n");
#endif
  
#ifdef MSWIN
  printf("Platform: MSWIN\n");
#endif
  
#ifdef UNIX
  printf("Platform: UNIX\n");
#endif

#ifdef LINUX
  printf("Platform: LINUX\n");
#endif
}



int main() {
  print();
  return 0;
}
