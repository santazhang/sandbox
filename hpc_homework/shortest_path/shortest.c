#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "MapReduceScheduler.h"

int N=1024;
int DIST=100;

#define M(a,b) m[(a)*N+(b)]

int * gen_graph(int n){
  int i, j;
  int * m=(int *)malloc(sizeof(int) *n*n);
  srand(1000);
  for (i = 0; i < n * n; i++) {
    m[i] = -1;
    if (i % n == i / n)
      m[i] = 0;
  }
  for(i=0;i<n;i++){
    for(j=0;j<n/5;j++){
      int dst=(i+rand())%n;
      while (dst == i) {
        dst = (i+rand())%n;
      }
      M(i,dst)=M(dst,i)=rand()%(DIST-1)+1;
    }
  }
  return m;
}


int path_shorter_than(int a, int b) {
  if (a == -1)
    return 0;
  else if (b == -1 && a != -1)
    return 1;
  else
    return a < b;
}

int path_add(int a, int b) {
  if (a == -1 || b == -1)
    return -1;
  else
    return a + b;
}

int* short_path_ford(int *map, int n) {
  int *dis = (int *) malloc(sizeof(int) * n);
  int i, j;
  int has_changed = 0;
  dis[0] = 0;
  for (i = 1; i < n; i++) {
    dis[i] = -1;// inf
  }
  for (;;) {
    has_changed = 0;
    for (i = 0; i < n; i++) {
      for (j = 0; j < n; j++) {
        if (j == i)
          continue;
        if (path_shorter_than(path_add(dis[j], map[i * n + j]), dis[i])) {
          //int old = dis[i];
          dis[i] = dis[j] + map[i * n + j];
          //printf("dist[%d] changed from %d to %d, (i,j)=%d,i=%d,j=%d\n", i, old, dis[i], map[i*n + j],i,j);
          has_changed = 1;
        }
      }
    }
    if (has_changed == 0)
      break;
  }
  

  return dis;
}

int my_comp(const void* k1, const void* k2) {
  int v1 = *(int*)k1;
  int v2 = *(int*)k2;
  if (v1 < v2)
    return -1;
  else if (v1 == v2)
    return 0;
  else
    return 1;
}


int* g_map;//global distance map, const
int g_nodes;

void my_map(map_args_t *args) {
  int i, j;

  int *data = (int*) (args->data);
  int *dis = data;// aliasing
/*
  printf("Good! map_arg_len = %d\n", args->length);
  printf("The data to be mapped:\n");
  for (i = 0; i < args->length; i++) {
    printf("%d\t", data[i]);
  }
  printf("\n");

  printf("Emitting intermediat values\n");
  */
  for (i = 0; i < g_nodes; i++) {
    for (j = 0; j < g_nodes; j++) {
      // emit  j as key, new distance from i -> j as value
      int *key = (int *)malloc(sizeof(int));
      int *val = (int *)malloc(sizeof(int));
      *key = j; // node id as key
//      printf("Key = %d, gnodes = %d\n", *key, g_nodes);
      *val = path_add(dis[i], g_map[i * g_nodes + j]);// distance from i -> j
  //    printf("val = %d\n", *val);
      
      if (path_shorter_than(*val , dis[j])) {
        // emit only if the solution is better
  //      printf("Emit: (%d, %d), path (%d->%d)\n", *key, *val, i, j);
        emit_intermediate(key, val, sizeof(int));
      }
    }
  }
}

void my_reduce(void* key_in, void** vals_in, int vals_len) {
  int i;
  int **vals = (int**)vals_in;//might have problem on 64bit system. better make a new array, and copy value into it
  //int key = *(int*) key_in;

  int *shortest = (int*)malloc(sizeof(int));
  *shortest = -1;

//  printf("Reduce: node id = %d\ndists=", key);
/*  for (i = 0; i < vals_len; i++) {
    printf("%d\t", *vals[i]);
  }
  printf("\n");
*/
  for (i = 0; i < vals_len; i++) {
    if (path_shorter_than(*vals[i], *shortest)) {
      *shortest = *vals[i];
    }
    free(vals[i]);// free values malloced in 'map' phase
  }

//  printf("Emitting last results: (%d,%d)\n", key, *shortest);
  emit(key_in, shortest);

}

void short_path_mapreduce_onepass(int *dis) {

  int i;
	scheduler_args_t sched_args;

	memset(&sched_args, 0, sizeof(sched_args));

	sched_args.task_data=(void*)dis;
	sched_args.map=my_map;
	sched_args.reduce=my_reduce;
	sched_args.splitter=NULL;
	sched_args.key_cmp=my_comp;
	sched_args.unit_size=sizeof(int);
	sched_args.partition=NULL;

	final_data_t result;
	sched_args.result=&result;

	sched_args.data_size=g_nodes*sizeof(int);

	sched_args.L1_cache_size = 1024 * 32;
	sched_args.num_map_threads = 8;
	sched_args.num_reduce_threads = 16;
	sched_args.num_merge_threads = 8;
	sched_args.num_procs = 2;
	sched_args.key_match_factor = 2;

  if (map_reduce_scheduler(&sched_args) < 0) {
    printf("Error in mapreduce!");
  }

  for (i = 0; i < result.length; i++) {
    keyval_t *curr = &((keyval_t *)result.data)[i];
  //  printf("(%d, %d)\n", *(int*)curr->key, *(int*)curr->val);

    dis[*(int*)curr->key] = *(int*)curr->val;
  }
}




int* short_path_mapreduce(int *map, int n) {
  int *dis = (int*) malloc(sizeof(int) * n);
  int *last_dis = (int*)malloc(sizeof(int) *n);
  int i;

  g_map = map;// set global map
  g_nodes = n;

  dis[0] = 0;//self visitable
  for (i = 1; i < n; i++) {
    dis[i] = -1;//inf
  }

  for (;;) {
    int has_changed = 0;// flag indicating whether distant map has changed after one map-reduce pass
    // save old value
    for (i = 0; i < n; i++) {
      last_dis[i] = dis[i];
    }

    short_path_mapreduce_onepass(dis);// do work

    // check if has change
    for (i = 0; i < n; i++) {
      if (last_dis[i] != dis[i]) {
        has_changed = 1;
        break;
      }
    }
    // break out of loop if nothing changed
    if (has_changed == 0) {
      break;
    }
  }


  return dis;
}


int main(){

  int * graph=gen_graph(N);
  int i;
/*  
  for (i = 0; i < N*N; i++) {
    printf("%d\t", graph[i]);
    if (i %N == N-1)
      printf("\n");
  }
*/
  // calculate min-distance from vertex 0 to the other vertices using Phoenix

  // print the distance of vertex 0 to vertex 1,2,3,...10

//  int *dis = short_path_ford(graph, N);

  int *dis_mapreduce = short_path_mapreduce(graph, N);

//  printf("The following result is generated by Ford algorithm, with out MapReduce\n");
/*
  for (i = 0; i < N && i < 10; i++) {
    if (dis[i] >= 0)
      printf("node %d: %d\n", i, dis[i]);
    else
      printf("node %d: unreachable\n", i);
  }
*/
//  printf("The following result is generated by Ford algorithm, with MapReduce\n");
  for (i = 0; i < N && i < 10; i++) {
    if (dis_mapreduce[i] >= 0)
      printf("node %d: %d\n", i, dis_mapreduce[i]);
    else
      printf("node %d: unreachable\n", i);
  }
  free(dis_mapreduce);
//  free(dis);
  free(graph);
  return 0;
}
