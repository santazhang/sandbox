#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <malloc.h>
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

void print_arr_int(int arr[], int len) {
  int i;
  for (i = 0; i < len; i++) {
    printf("%d ", arr[i]);
  }
  printf("\n");
}

void do_merge(int arr[], int half, int len) {
  int* p_buf = (int *) malloc(sizeof(int) * len);
  int i, j, k;
  i = 0;
  j = half;
  k = 0;
  while (i < half && j < len) {
    if (arr[i] < arr[j]) {
      p_buf[k] = arr[i];
      i++;
      k++;
    } else {
      p_buf[k] = arr[j];
      j++;
      k++;
    }
  }
  while (i < half) {
    p_buf[k] = arr[i];
    i++;
    k++;
  }
  while (j < len) {
    p_buf[k] = arr[j];
    j++;
    k++;
  }
  for (i = 0; i < len; i++) {
    arr[i] = p_buf[i];
  }
  free(p_buf);
}

void merge_sort_int(int arr[], int len) {
  if (len < 5) {
    select_sort_int(arr, len);
  } else {
    int half = len / 2;
    merge_sort_int(arr, half);
    merge_sort_int(arr + half, len - half);
    
    do_merge(arr, half, len);
  }
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
  
  printf("Start merge sorting...\n");
  merge_sort_int(p_arr, len);
  printf("Finished merge sorting\n");
  
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
