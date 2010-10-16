/*************************************************************************
* Author: Santa Zhang
* Created Time: 2009-11-11 22:33:10
* File Name: nbody.c
* Description: nBody homework
 ************************************************************************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "MapReduceScheduler.h"

/* the vector type */
typedef struct {
  float x;
  float y;
  float z;
} vec_t;

/* some freaking parameters */
float g_time_step = 0.0;
float g_cluster_scale = 0.0;
float g_velocity_scale = 0.0;
float g_softening = 0.0;
float g_damping = 0.0;

/* number of nodes */
int g_num_nodes = 0;

/* position of each node */
vec_t* g_pos = NULL;

/* velocity of each node */
vec_t* g_vel = NULL;

/* acceleration of each node */
vec_t* g_acc = NULL;

/* mass of each node */
float* g_mass = NULL;

/* 1/(mass) for each node */
float* g_inv_mass = NULL;

void read_input(char *fname) {
  int i;
  int ret;
  FILE *fp = fopen(fname, "r");
  if (fp == NULL) {
    printf("failed to open input file '%s'!\n", fname);
    exit(1);
  }

  ret = fscanf(fp, "%f", &g_time_step);
  ret = fscanf(fp, "%f", &g_cluster_scale);
  ret = fscanf(fp, "%f", &g_velocity_scale);
  ret = fscanf(fp, "%f", &g_softening);
  ret = fscanf(fp, "%f", &g_damping);
  ret = fscanf(fp, "%d", &g_num_nodes);

  printf("input params:\n");
  printf("timestep=%f\n", g_time_step);
  printf("cluster_scale=%f\n", g_cluster_scale);
  printf("velocity_scale=%f\n", g_velocity_scale);
  printf("softening=%f\n", g_softening);
  printf("damping=%f\n", g_damping);
  printf("# nodes=%d\n", g_num_nodes);

  g_pos = (vec_t *) malloc(sizeof(vec_t) * g_num_nodes);
  g_vel = (vec_t *) malloc(sizeof(vec_t) * g_num_nodes);
  g_acc = (vec_t *) malloc(sizeof(vec_t) * g_num_nodes);
  g_mass = (float *) malloc(sizeof(float) * g_num_nodes);
  g_inv_mass = (float *) malloc(sizeof(float) * g_num_nodes);

  for (i = 0; i < g_num_nodes; i++) {
    ret = fscanf(fp, "%f", &g_pos[i].x);
    ret = fscanf(fp, "%f", &g_pos[i].y);
    ret = fscanf(fp, "%f", &g_pos[i].z);
    ret = fscanf(fp, "%f", &g_mass[i]);
  }

  for (i = 0; i < g_num_nodes; i++) {
    ret = fscanf(fp, "%f", &g_vel[i].x);
    ret = fscanf(fp, "%f", &g_vel[i].y);
    ret = fscanf(fp, "%f", &g_vel[i].z);
    ret = fscanf(fp, "%f", &g_inv_mass[i]);
  }

  fclose(fp);
}

int key_cmp(const void *key1, const void *key2) {
  int n1 = *(int *) key1;
  int n2 = *(int *) key2;
  if (n1 < n2)
    return -1;
  else if (n1 == n2)
    return 0;
  else
    return 0;
}

void map_worker(map_args_t *args) {
  int idx;
  int i, j;

  for (idx = 0; idx < args->length; idx++) {
    i = ((int *) args->data)[idx];
    int *key = (int *) malloc(sizeof(int));
    *key = i;

    g_acc[i].x = 0.0;
    g_acc[i].y = 0.0;
    g_acc[i].z = 0.0;
    for (j = 0; j < g_num_nodes; j++) {
      float r[3], dist2, inv_dist, inv_dist_cube, s;
      r[0] = g_pos[j].x - g_pos[i].x;
      r[1] = g_pos[j].y - g_pos[i].y;
      r[2] = g_pos[j].z - g_pos[i].z;
      dist2 = r[0] * r[0] + r[1] * r[1] + r[2] * r[2];
      dist2 += g_softening * g_softening;
      inv_dist = 1.0f / sqrtf(dist2);
      inv_dist_cube = inv_dist * inv_dist * inv_dist;
      s = g_mass[j] * inv_dist_cube;

      g_acc[i].x += r[0] * s;
      g_acc[i].y += r[1] * s;
      g_acc[i].z += r[2] * s;
    }

    // emit (key=node, val=nothing)
    emit_intermediate(key, NULL, sizeof(int));
  }
}

void reduce_worker(void *key_in, void **vals_in, int vals_len) {
  int i = *(int *) key_in;

  g_vel[i].x += g_acc[i].x * g_time_step;
  g_vel[i].y += g_acc[i].y * g_time_step;
  g_vel[i].z += g_acc[i].z * g_time_step;

  g_vel[i].x *= 0.995;//g_damping; // XXX the original code uses fixed damping value!!!
  g_vel[i].y *= 0.995;//g_damping;
  g_vel[i].z *= 0.995;//g_damping;

  g_pos[i].x += g_vel[i].x * g_time_step;
  g_pos[i].y += g_vel[i].y * g_time_step;
  g_pos[i].z += g_vel[i].z * g_time_step;

  emit(key_in, &g_pos[i]);
}

void output() {
  int i;
  int count = (g_num_nodes > 10) ? 10 : g_num_nodes;
  for (i = 0; i < count; i++) {
    printf("(%f, %f, %f)\n", g_pos[i].x, g_pos[i].y, g_pos[i].z);
  }
}

void release_resource() {
  free(g_pos);
  free(g_vel);
  free(g_acc);
  free(g_mass);
  free(g_inv_mass);
  printf("released allocated resource\n");
}

int main(int args, char *argv[]) {
  int i, j;
  int iter = 0;
  int *index_arr;
  scheduler_args_t sched_args;
  final_data_t result;

  if (args != 3) {
    printf("Usage: nbody <input-file> <num-iterations>\n");
    exit(1);
  }
  read_input(argv[1]);
  iter = atoi(argv[2]);

  memset(&sched_args, 0, sizeof(sched_args));

  index_arr = (int *) malloc(sizeof(int) * g_num_nodes);
  for (i = 0; i < g_num_nodes; i++) {
    index_arr[i] = i;
  }

  sched_args.task_data = index_arr;
  sched_args.map = map_worker;
  sched_args.reduce = reduce_worker;
  sched_args.splitter = NULL;
  sched_args.key_cmp = key_cmp;
  sched_args.unit_size = sizeof(int);
  sched_args.partition = NULL;
  sched_args.result = &result;
  sched_args.data_size = sizeof(int) * g_num_nodes;
  sched_args.num_map_threads = 8;

  // tricky here!
  if (sched_args.data_size / sched_args.num_map_threads > 1024 * 32) {
    sched_args.L1_cache_size = 1024 * 32;
  } else {
    sched_args.L1_cache_size = sched_args.data_size / sched_args.num_map_threads;
  }

  sched_args.num_reduce_threads = 16;
  sched_args.num_merge_threads = 8;
  sched_args.num_procs = 2;
  sched_args.key_match_factor = 2;

  for (i = 0; i < iter; i++) {
    map_reduce_scheduler(&sched_args);
    for (j = 0; j < result.length; j++) {
      free(result.data[j].key);
    }
  }
  output();
  free(index_arr);
  release_resource();
  return 0;
}

