#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "MapReduceScheduler.h"
#include "stddefines.h"

const int NUM_INTS=100;
const int MODE=20;


void gen_ints(int * ints, unsigned int n){
  unsigned i;
	srand(1000);
	for(i=0;i<n;i++){
		ints[i]=rand()%MODE;
	}
}

int compare(const void * s1, const void * s2){
	int i1=*(int*)s1;
	int i2=*(int*)s2;
	if(i1<i2)
		return -1;
	if(i1>i2)
		return 1;
	return 0;
}


void map(map_args_t * args){
  int i;
	for (i = 0; i < args->length; i++) {
		emit_intermediate(((int *)args->data) + i, NULL, sizeof(int));
	}
}

void reduce(void * key_in, void ** vals_in, int vals_len){
	int * val=(int*)malloc(sizeof(int));
	*val=vals_len;
//	printf("*key_in = %d, val=%d\n", *(int*)key_in, *val);
	emit(key_in, (void *)val);
}

int main(){

	int * ints=(int*)malloc(sizeof(int) * NUM_INTS);
  int i;
	gen_ints(ints, NUM_INTS);

	scheduler_args_t sched_args;
	memset(&sched_args, 0, sizeof(sched_args));
	sched_args.task_data=(void*)ints;
	sched_args.map=map;
	sched_args.reduce=reduce;
	sched_args.splitter=NULL;
	sched_args.key_cmp=compare;
	sched_args.unit_size=sizeof(int);
	sched_args.partition=NULL;

	final_data_t result;
	sched_args.result=&result;

	sched_args.data_size=NUM_INTS*sizeof(int);

	sched_args.L1_cache_size = 1024 * 32;
	sched_args.num_map_threads = 8;
	sched_args.num_reduce_threads = 16;
	sched_args.num_merge_threads = 8;
	sched_args.num_procs = 2;
	sched_args.key_match_factor = 2;

	CHECK_ERROR(map_reduce_scheduler(&sched_args) < 0);
	
	for(i=0;i<10 && i<result.length;i++){
		keyval_t * curr=&((keyval_t *)result.data)[i];
    printf("(%d,%d)\n", *(int*)curr->key, *(int*)curr->val);
		free((int*)curr->val);
	}

  free(ints);
	return 0;
}
