#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#ifndef __APPLE__
#include <malloc.h>
#endif  // __APPLE__

#include <math.h>

void select_sort_int(int arr[], int len) {
  int i, j, t, min, min_index;
  for (i = 0; i < len; i++) {
    min_index = i;
    min = arr[min_index];
    for (j = i + 1; j < len; j++) {
      if (arr[j] < min) {
        min_index = j;
        min = arr[min_index];
      }
    }
    if (min_index != i) {
      t = arr[i];
      arr[i] = arr[min_index];
      arr[min_index] = t;
    }
  }
}

void bubble_sort_int(int arr[], int len) {
  int i, j, t;
  for (i = 0; i < len; i++) {
    for (j = i + 1; j < len; j++) {
      if (arr[i] > arr[j]) {
        t = arr[i];
        arr[i] = arr[j];
        arr[j] = t;
      }
    }
  }
}

void print_arr_int(int arr[], int len) {
  int i;
  for (i = 0; i < len; i++) {
    printf("%d ", arr[i]);
  }
  printf("\n");
}

void qsort_int_not_recursive(int arg_arr[], int arg_len) {
  int** p_arr_stack = (int **) malloc(sizeof(int *) * 100);
  int* p_len_stack = (int *) malloc(sizeof(int) * 100);
  int stack_ptr = 0;
  int max_depth = 0;
  
  p_arr_stack[stack_ptr] = arg_arr;
  p_len_stack[stack_ptr] = arg_len;
  stack_ptr++;
  
  while (stack_ptr > 0) {
    int *arr;
    int len;
  
    stack_ptr--;
    arr = p_arr_stack[stack_ptr];
    len = p_len_stack[stack_ptr];
    
    if (max_depth < stack_ptr) {
      max_depth = stack_ptr;
      printf("Max depth reached %d\n", max_depth);
    }
    
    if (len < 5) {
      select_sort_int(arr, len);
    } else {
      int i, j, k, t;
      int key_index = rand() % len;
      int key = arr[key_index];
      
      // 1st phase, push all elements smaller than 'key' to the front
      j = 0;
      for (i = 0; i < len; i++) {
        if (arr[i] < key) {
          t = arr[j];
          arr[j] = arr[i];
          arr[i] = t;
          j++;
        }
      }
      
      // 2nd phase, push all elements equal to 'key' to the middle
      k = j;
      for (i = j; i < len; i++) {
        if (arr[i] == key) {
          t = arr[i];
          arr[i] = arr[k];
          arr[k] = t;
          k++;
        }
      }
      
      // recurisvely call qsort procedure
      if (j > 1) {
        p_arr_stack[stack_ptr] = arr;
        p_len_stack[stack_ptr] = j;
        stack_ptr++;
      }
      
      if (len - k > 1) {
        p_arr_stack[stack_ptr] = arr + k;
        p_len_stack[stack_ptr] = len - k;
        stack_ptr++;
      }
    }
  }
  
  free(p_arr_stack);
  free(p_len_stack);
}

int depth = 0;
int max_depth = 0;

void qsort_int(int arr[], int len) {

  depth++;
  
  if (max_depth < depth) {
    printf("Q-sort recusive depth reached %d\n", depth);
    max_depth = depth;
  }

  if (len < 5) {
    select_sort_int(arr, len);
  } else {
    int i, j, k, t;
    int key_index = rand() % len;
    int key = arr[key_index];
    
    // 1st phase, push all elements smaller than 'key' to the front
    j = 0;
    for (i = 0; i < len; i++) {
      if (arr[i] < key) {
        t = arr[j];
        arr[j] = arr[i];
        arr[i] = t;
        j++;
      }
    }
    
    // 2nd phase, push all elements equal to 'key' to the middle
    k = j;
    for (i = j; i < len; i++) {
      if (arr[i] == key) {
        t = arr[i];
        arr[i] = arr[k];
        arr[k] = t;
        k++;
      }
    }
    
    // recurisvely call qsort procedure
    if (j > 1) {
      qsort_int(arr, j);
    }
    
    if (len - k > 1) {
      qsort_int(arr + k, len - k);
    }
  }
  
  depth--;
}

int check_arr_sorted(int arr[], int len) {
  int i;
  for (i = 0; i < len - 1; i++) {
    if (arr[i] > arr[i + 1]) {
      return 0;
    }
  }
  return 1;
}

int main(int argc, char* argv[]) {
  int *p_arr;
  int len;
  int i;
  
  srand(time(NULL));
  
  printf("A random array will be generated, tell me the array size:\n");
  fflush(0);  // flush stdout
  scanf("%d", &len);
  
  if (len < 2) {
    printf("Length not acceptable, quit.\n");
    exit(1);
  }
  
  printf("Populating an array with length %d\n", len);
  p_arr = (int *) malloc(sizeof(int) * len);
  
  for (i = 0; i < len; i++) {
    p_arr[i] = rand() % (len - 1);
  }
  
  printf("Start Q-sorting...\n");
  qsort_int_not_recursive(p_arr, len);
  printf("Finished Q-sorting\n");
  printf("Theoretical recursive depth is %d\n", (int) (log(len) / log(2)));
  
  if (len <= 100) {
    printf("The result:\n");
    print_arr_int(p_arr, len);
  } else {
    printf("The array is too long, so the result is not displayed\n");
  }
  
  if (check_arr_sorted(p_arr, len) == 1) {
    printf("The array is correctly sorted.\n");
  } else {
    printf("The array is NOT correctly sorted.\n");
  }
  
  free(p_arr);
  
  return 0;
}
