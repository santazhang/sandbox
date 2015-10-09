#ifndef _NUMA_H
#define _NUMA_H 1

#include <stddef.h>

#define LIBNUMA_API_VERSION 1

#ifndef __APPLE__
#error This is only a workaround for MACOSX
#endif  // __APPLE__

#ifdef __cplusplus
extern "C" {
#endif

int numa_available(void);
int numa_max_node(void);
int numa_run_on_node(int node);
void numa_set_preferred(int node);
void numa_tonode_memory(void *start, size_t size, int node);
int numa_num_configured_nodes();

#ifdef __cplusplus
}
#endif

#endif
