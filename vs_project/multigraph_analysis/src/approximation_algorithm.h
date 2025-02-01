#ifndef APPROXIMATION_ALGORITHM_H
#define APPROXIMATION_ALGORITHM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "graph_algorithm.h"
#include "utils.h"

/* These constants appear in approximation_algorithm.c for the simulated annealing approach */
#define MAX_ITER       20000
#define INITIAL_TEMP   200.0
#define COOLING_RATE   0.99

/*
 * approximate_find_cycles
 * -----------------------
 * A simple approximate method that looks for cycles in a portion of the graph.
 * Writes found cycles into output_cycles and cycle_sizes, increments cycle_count.
 */
int approximate_find_cycles(
    void* graph,
    int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count
);

/*
 * approximate_count_hamiltonian_cycles
 * ------------------------------------
 * Approximates the number of Hamiltonian cycles, storing them if found.
 */
int approximate_count_hamiltonian_cycles(
    void* graph,
    int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count
);

/*
 * generate_random_permutation
 * ---------------------------
 * Used internally to shuffle an array for the simulated annealing approach.
 */
void generate_random_permutation(int* arr, int n);

/*
 * approximate_required_operations
 * -------------------------------
 * Approximates the “distance” (or difference) in edges when matching two
 * graphs under a particular permutation of vertices.
 * Called by approximate_calculate_metric (or related logic).
 */
int approximate_required_operations(
    GraphAlgorithmContext* context_1,
    GraphAlgorithmContext* context_2,
    int* arr,
    int n,
    int smaller_n
);

/*
 * approximate_calculate_metric
 * ----------------------------
 * An approximate “similarity” or “distance” measure between two graphs.
 * Uses a random / simulated-annealing approach to minimize operations.
 */
int approximate_calculate_metric(
    GraphAlgorithmContext* context_1,
    GraphAlgorithmContext* context_2,
    int vertices_1,
    int vertices_2
);

/*
 * approximate_find_minimal_extension
 * ----------------------------------
 * Approximates how many edges we need to add to create at least one Hamiltonian cycle.
 */
int approximate_find_minimal_extension(void* graph, int vertices);

/*
 * approximate_count_maximal_cycles
 * --------------------------------
 * Approximates the length of the longest cycle in the graph.
 */
int approximate_count_maximal_cycles(void* graph, int vertices);

/*
 * approximate_find_maximal_cycles
 * -------------------------------
 * Approximates all cycles of maximum length, storing them in output_cycles, etc.
 */
int approximate_find_maximal_cycles(
    void* graph,
    int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count
);

#endif /* APPROXIMATION_ALGORITHM_H */
