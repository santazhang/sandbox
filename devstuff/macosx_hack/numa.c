#include "numa.h"

#ifndef __APPLE__
#error This is only a workaround for MACOSX
#endif  // __APPLE__

int numa_available(void) {
    return 0;
}

int numa_max_node(void) {
    return 1;
}

int numa_run_on_node(int node) {
    return 0;
}

void numa_set_preferred(int node) {
    return;
}

void numa_tonode_memory(void *start, size_t size, int node) {
    return;
}

int numa_num_configured_nodes() {
    return 1;
}
