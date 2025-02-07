#ifndef APPROXIMATION_ALGORITHM_H
#define APPROXIMATION_ALGORITHM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "graph_algorithm.h"
#include "graph_algorithm_context.h"
#include "utils.h"

#define MAX_ITER       20000
#define INITIAL_TEMP   200.0

int approximate_required_operations(
    GraphAlgorithmContext* context_1,
    GraphAlgorithmContext* context_2,
    int* arr,
    int n,
    int smaller_n
);

void generate_random_permutation(int* arr, int n);

int approximate_calculate_metric(
    GraphAlgorithmContext* context_1,
    GraphAlgorithmContext* context_2,
    int vertices_1,
    int vertices_2
);

int approximate_find_minimal_extension(void* graph, int vertices);

int approximate_count_maximal_cycles(void* graph, int vertices);

int approximate_find_maximal_cycles(void* graph, int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count);
/*
    Approximation algorithms for cycle and path partitions in complete graphs https://arxiv.org/pdf/2311.11332
*/
int approximate_find_cycles(void* graph, int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count);


int approximate_count_hamiltonian_cycles(void* graph, int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count);


#endif /* APPROXIMATION_ALGORITHM_H */
