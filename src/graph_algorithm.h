#ifndef GRAPH_ALGORITHM_H
#define GRAPH_ALGORITHM_H

#include "graph.h"

typedef struct {
    int (*calculate_size)(Graph *graph);
    int (*find_cycles)(Graph *graph);
    int (*count_hamiltonian_cycles)(Graph *graph);
} GraphAlgorithm;

extern GraphAlgorithm default_algorithm;

#endif
