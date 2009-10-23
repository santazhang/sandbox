#include <stdio.h>

int main() {
  int low, high;
  int guess;
  char desc[10];
  
  low = 1;
  high = 10;
  
  for (;;) {
    
    scanf("%d", &guess);
    
    if (guess == 0)
      break;
    scanf("%s", desc);
    scanf("%s", desc);
    if (desc[0] == 'o') {
      if (low <= guess && guess <= high) {
        printf("Stan may be honest\n");
      } else {
        printf("Stan is dishonest\n");
      }
      
      low = 1;
      high = 10;
    } else if (desc[0] == 'h') {
      
      if (high > guess - 1)
        high = guess - 1;
    } else if (desc[0] == 'l') {
      if (low < guess + 1)
        low = guess + 1;
    }
    
  }
  return 0;
}
