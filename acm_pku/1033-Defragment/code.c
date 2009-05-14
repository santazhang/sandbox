#include <stdio.h>

int main(int argc, char *argv[])
{
  int N, K, i, j, counter, needs_defrag;
  int **file_clusters;
  int *file_size;
  int *cluster_dest;
  int *occupy_flag;
  int *cluster_src;
  scanf("%d", &N);
  scanf("%d", &K);
  file_clusters = (int **) malloc(sizeof(int *) * (N + 1));
  file_size = (int *) malloc(sizeof(int) * (N + 1));
  cluster_dest = (int *) malloc(sizeof(int) * (N + 1));
  cluster_src = (int *) malloc(sizeof(int) * (N + 1));
  occupy_flag = (int *) malloc(sizeof(int) * (N + 1));
  for (i = 1; i <= N; i++) {
    cluster_dest[i] = i;
    cluster_src[i] = i;
    occupy_flag[i] = 0;
  }
  counter = 1;
  for (i = 1; i <= K; i++) {
    scanf("%d", &file_size[i]);
    file_clusters[i] = (int *) malloc(sizeof(int) * file_size[i]);
    for (j = 0; j < file_size[i]; j++) {
      scanf("%d", &file_clusters[i][j]);
      occupy_flag[file_clusters[i][j]] = 1;
      cluster_dest[file_clusters[i][j]] = counter;
      cluster_src[counter] = file_clusters[i][j];
      counter++;
    }
  }

  needs_defrag = 0;
  for (i = 1; i <= N; i++) {
    if (cluster_dest[i] != i) {
      needs_defrag = 1;
      break;
    }
  }

  if (needs_defrag == 0) {
    printf("No optimization needed\n");
    return 0;
  }

  for (i = N; i > 0; i--) {
    if (cluster_dest[i] != i) {
      // check if is a loop
      int is_loop = 0;
      int last_sec = i;
      int sec = cluster_dest[last_sec];
      while (sec != last_sec && sec != cluster_dest[sec]) {
        sec = cluster_dest[sec];
      }
      if (sec == last_sec) {
        // is a loop
        int swap = 0;
        int sec_next;
        is_loop = 1;
        //printf("Found loop on sec %d\n", last_sec);
        for (j = N; j > 0; j--) {
          if (occupy_flag[j] == 0) {
            swap = j;
            break;
          }
        }
        sec = cluster_src[last_sec];
        printf("%d %d\n", sec, swap);
        cluster_dest[sec] = sec;
        while (sec != last_sec) {
          sec_next = cluster_src[sec];
          printf("%d %d\n", sec_next, sec);
          sec = sec_next;
          cluster_dest[sec] = sec;
        }
        printf("%d %d\n", swap, sec);
      } else {
        is_loop = 0;
        //printf("Found list on sec %d\n", last_sec);
        while (sec != last_sec) {
          sec = cluster_src[sec];
          printf("%d %d\n", sec, cluster_dest[sec]);
          occupy_flag[sec] = 0;
          occupy_flag[cluster_dest[sec]] = 1;
          cluster_dest[sec] = sec;
        }
      }
    }
  }

  // no mem free, I'm lazy
  return 0;
}

