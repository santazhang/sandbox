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

void sift_up(int arr[], int pos, int len) {
  int val = arr[pos];
  int parent = (pos - 1) / 2;

  while (pos > 0) {
    
    if (arr[parent] < val) {
      arr[pos] = arr[parent];
    } else {
      break;
    }

    pos = parent;
    parent = (pos - 1) / 2;
  }

  arr[pos] = val;

}


void sift_down(int arr[], int pos, int len) {
  int val = arr[pos]; // the value to be sifted down
  int idx = pos;

  // while not reached bottom level
  while (idx < len / 2) {
    int left_idx = idx * 2 + 1; // index of left child
    int right_idx = idx * 2 + 2;  // index of right child

    // if only has left child
    if (right_idx == len) {
      // and the left child has a bigger value
      if (arr[left_idx] > val) {
        arr[idx] = arr[left_idx];
      }
      idx = left_idx;
      break;

    } else {
      // has both left and right child

      // left child is largest
      if (arr[left_idx] >= arr[right_idx] && arr[left_idx] > val) {
        arr[idx] = arr[left_idx];
        idx = left_idx;

      // right child is largest
      } else if (arr[right_idx] >= arr[left_idx] && arr[right_idx] > val) {
        arr[idx] = arr[right_idx];
        idx = right_idx;

      // val is largest
      } else {
        break;
      }
    }
  }

  // write the sifted down value to the correct position
  arr[idx] = val;
}


int check_heap_order(int arr[], int len) {
  int i;
  for (i = 0; i < len / 2; i++) {
    if (i * 2 + 1 < len && arr[i] < arr[i * 2 + 1]) {
      // if has left child, and is smaller than left child
      return 0;
    }
    if (i * 2 + 2 < len && arr[i] < arr[i * 2 + 2]) {
      // if has right child, and is smaller than right child
      return 0;
    }
  }
  return 1;
}

void build_heap(int arr[], int len) {
  int i;
//  print_arr_int(arr, len);
  for (i = len / 2 - 1; i >= 0; i--) {
    sift_down(arr, i, len);
  }
//  print_arr_int(arr, len);
  if (check_heap_order(arr, len) == 0) {
    printf("Crap! not in heap order!\n");
    exit(1);
  }
}

void heap_push(int arr[], int len, int val) {
  arr[len] = val;
  sift_up(arr, len, len + 1);
}


int heap_pop(int arr[], int len) {
  int ret = arr[0];
  arr[0] = arr[len - 1];
  sift_down(arr, 0, len - 1);
  return ret;
}


void heap_sort_int(int arr[], int len) {
  build_heap(arr, len);
  while (len > 1) {
    arr[len - 1] = heap_pop(arr, len);
    len--;
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
  
  printf("Start heap sorting...\n");
  heap_sort_int(p_arr, len);
  printf("Finished heap sorting\n");
  
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

  for (i = 0; i < len; i++) {
    int v = rand() % (len - 1);
//    printf("Pushing %d\n", v);
    heap_push(p_arr, i, v);
//    print_arr_int(p_arr, i + 1);
  }


  if (check_heap_order(p_arr, len)) {
    printf("Passed heap tests\n");
  } else {
    printf("Heap test FAILED\n");
  }


  
  free(p_arr);
  
  return 0;
}
