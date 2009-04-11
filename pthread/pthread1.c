#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <malloc.h>

pthread_mutex_t mutex;

void *echo_id(void *p_val) {
  usleep(rand() % 3000);
  pthread_mutex_lock(&mutex);
  printf("This is thread ");
  usleep(rand() % 3000);
  printf("%d\n", *(int *) p_val);
  free(p_val);
  pthread_mutex_unlock(&mutex);
}


int main() {
  int i;
  const int test_threads = 20;
  pthread_t th[test_threads];
  srand(time(NULL));
  pthread_mutex_init(&mutex, NULL);
  for (i = 0; i < test_threads; i++) {
    int *v = (int *) malloc(sizeof(int));
    *v = i;
    pthread_create(&th[i], NULL, echo_id, v);
  }
  sleep(1);
  pthread_mutex_destroy(&mutex);
  return 0;
}

