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

void update() {
  int i, j;
  for (i = 0; i < g_num_nodes; i++) {
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
  }

  // update location & vel
  for (i = 0; i < g_num_nodes; i++) {
    g_vel[i].x += g_acc[i].x * g_time_step;
    g_vel[i].y += g_acc[i].y * g_time_step;
    g_vel[i].z += g_acc[i].z * g_time_step;

    g_vel[i].x *= 0.995;//g_damping; // XXX the original code uses fixed damping value!!!
    g_vel[i].y *= 0.995;//g_damping;
    g_vel[i].z *= 0.995;//g_damping;

    g_pos[i].x += g_vel[i].x * g_time_step;
    g_pos[i].y += g_vel[i].y * g_time_step;
    g_pos[i].z += g_vel[i].z * g_time_step;
  }
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
  int i;
  int iter = 0;
  if (args != 3) {
    printf("Usage: nbody <input-file> <num-iterations>\n");
    exit(1);
  }
  read_input(argv[1]);
  iter = atoi(argv[2]);
  for (i = 0; i < iter; i++) {
    update();
  }
  output();
  release_resource();
  return 0;
}

