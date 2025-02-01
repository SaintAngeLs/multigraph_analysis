#ifndef APPROXIMATION_ALGORITHM_H
#define APPROXIMATION_ALGORITHM_H

#include <stdio.h>
#include "glib.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "graph_algorithm.h"


int approximate_find_cycles(void *graph, int vertices, GArray *output_cycles);

int approximate_count_hamiltonian_cycles(void *graph, int vertices, GArray *output_cycles);

int approximate_calculate_metric(GraphAlgorithmContext *context_1,  GraphAlgorithmContext *context_2, int vertices_1, int vertices_2);

int approximate_find_minimal_extension(void *graph, int vertices);

int approximate_count_maximal_cycles(void *graph, int vertices);

int approximate_find_maximal_cycles(void *graph, int vertices, GArray *output_cycles);

#endif // APPROXIMATION_ALGORITHM_H

