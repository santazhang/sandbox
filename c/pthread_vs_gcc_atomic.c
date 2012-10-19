#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void dummy_call() {
}

int main(int argc, char* argv[]) {
    printf("you should have compiled me with -pthread -lpthread\n");
    int n = 1000 * 1000 * 10;
    int i;
    if (argc > 1) {
        n = atoi(argv[1]);
    }
    printf("%d operations\n", n);

    struct timeval start, stop;

    int k = 0;
    printf("doing basic incr...\n");
    gettimeofday(&start, NULL);
    for (i = 0; i < n; i++) {
        k++;
    }
    gettimeofday(&stop, NULL);
    double t = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1000.0 / 1000.0;
    printf("time used: %lf, val = %d\n", t, k);

    k = 0;

    printf("doing basic incr with dummy func call...\n");
    gettimeofday(&start, NULL);
    for (i = 0; i < n; i++) {
        dummy_call();
        k++;
        dummy_call();
    }
    gettimeofday(&stop, NULL);
    t = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1000.0 / 1000.0;
    printf("time used: %lf, val = %d\n", t, k);
    k = 0;

    printf("doing atomic incr (add and fetch)...\n");
    gettimeofday(&start, NULL);
    for (i = 0; i < n; i++) {
        __sync_add_and_fetch(&k, 1);
    }
    gettimeofday(&stop, NULL);
    t = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1000.0 / 1000.0;
    printf("time used: %lf, val = %d\n", t, k);
    k = 0;
    printf("doing atomic incr 2 (fetch and add)...\n");
    gettimeofday(&start, NULL);
    for (i = 0; i < n; i++) {
        __sync_fetch_and_add(&k, 1);
    }
    gettimeofday(&stop, NULL);
    t = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1000.0 / 1000.0;
    printf("time used: %lf, val = %d\n", t, k);
    k = 0;
    printf("doing atomic compare-and-swap...\n");
    gettimeofday(&start, NULL);
    for (i = 0; i < n; i++) {
        int k2 = k + 1;
        __sync_bool_compare_and_swap(&k, k, k2);
    }
    gettimeofday(&stop, NULL);
    t = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1000.0 / 1000.0;
    printf("time used: %lf, val = %d\n", t, k);
    k = 0;
    printf("doing atomic test and set...\n");
    gettimeofday(&start, NULL);
    for (i = 0; i < n; i++) {
        int k2 = k + 1;
        __sync_lock_test_and_set(&k, k2);
    }
    gettimeofday(&stop, NULL);
    t = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1000.0 / 1000.0;
    printf("time used: %lf, val = %d\n", t, k);
    printf("doing pthread incr...\n");

    pthread_mutex_t m;
    k = 0;
    pthread_mutex_init(&m, NULL);
    gettimeofday(&start, NULL);
    for (i = 0; i < n; i++) {
        pthread_mutex_lock(&m);
        k++;
        pthread_mutex_unlock(&m);
    }
    gettimeofday(&stop, NULL);
    t = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1000.0 / 1000.0;
    printf("time used: %lf, val = %d\n", t, k);
    pthread_mutex_destroy(&m);

    return 0;
}
