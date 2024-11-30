#ifndef GRAPH_ALGORITHM_H
#define GRAPH_ALGORITHM_H

#include <glib.h>
#include "graph_interface.h"


typedef struct {

    GraphInterface *graph_interface;

    int vertices;

} GraphAlgorithmContext;


GraphAlgorithmContext* create_context(void *graph, int vertices);

void destroy_context(GraphAlgorithmContext *context);


typedef struct {

    int (*calculate_size)(void *graph);

    int (*find_cycles)(void *graph, int vertices);

    int (*count_hamiltonian_cycles)(void *graph, int vertices);

    GArray* (*calculate_metric)(void *graph, int vertices);

    int (*find_minimal_extension)(void *graph, int vertices);

} GraphAlgorithm;


extern GraphAlgorithm default_algorithm;

#endif
