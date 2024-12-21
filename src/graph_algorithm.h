#ifndef GRAPH_ALGORITHM_H
#define GRAPH_ALGORITHM_H

#include <stdio.h>
#include <glib.h>
#include "graph_interface.h"
#include <stdbool.h>
#include <stdlib.h>


typedef struct {

    GraphInterface *graph_interface;

    int vertices;

} GraphAlgorithmContext;


GraphAlgorithmContext* create_context(void *graph, int vertices);

void destroy_context(GraphAlgorithmContext *context);


typedef struct {

    int (*calculate_size)(void *graph);

    int (*find_cycles)(void *graph, int vertices, GArray *output_cycles);

    int (*count_hamiltonian_cycles)(void *graph, int vertices, GArray *output_cycles);

    double (*calculate_metric)(void *graph_1, int vertices_1, void *graph_2, int vertices_2);

    int (*find_minimal_extension)(void *graph, int vertices);

    int (*count_maximal_cycles)(void *graph, int vertices);

    int (*find_maximal_cycles)(void *graph, int vertices, GArray *output_cycles);

} GraphAlgorithm;


extern GraphAlgorithm default_algorithm;

#endif // GRAPH_ALGORITHM_H
