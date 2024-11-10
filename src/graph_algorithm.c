#include "graph_algorithm.h"
#include "graph.h"

static int calculate_size(Graph *graph) {
    return 0;
}

static int find_cycles(Graph *graph) {
    return 0;
}

static int count_hamiltonian_cycles(Graph *graph) {
    return 0;
}

GraphAlgorithm default_algorithm = {
    .calculate_size = calculate_size,
    .find_cycles = find_cycles,
    .count_hamiltonian_cycles = count_hamiltonian_cycles,
};
