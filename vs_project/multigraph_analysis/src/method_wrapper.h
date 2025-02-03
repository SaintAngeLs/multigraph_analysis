#ifndef METHOD_WRAPPER_H
#define METHOD_WRAPPER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include "graph.h" 

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

#include "utils.h"
#include "graph_algorithm_context.h"
#include "string_list.h"
#include "cycle_list.h"
#include "stack.h"
#include "time.h"
#include "graph_algorithm.h"
#include "approximation_algorithm.h"
#include "common_utils.h"

#define THRESHOLD     10  

int calculate_size_wrapper(void* graph);

int find_cycles_wrapper(
    void* graph,
    int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count);

int count_hamiltonian_cycles_wrapper(
    void* graph,
    int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count);

void calculate_metric_wrapper(
    void* g1, int v1,
    void* g2, int v2,
    int* exact_metric,
    int* approximate_metric);
/*
 * EXACT: find_minimal_extension_wrapper
 */
int find_minimal_extension_wrapper(void* graph, int vertices);
/*
 * EXACT: count_maximal_cycles_wrapper => length of the longest simple cycle
 */
int count_maximal_cycles_wrapper(void* graph, int vertices);

/*
 * EXACT: find_maximal_cycles_wrapper => enumerates all cycles that match
 * the maximum cycle length found above.
 */
int find_maximal_cycles_wrapper(
    void* graph,
    int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count
);

#endif /* METHOD_WRAPPER_H */
