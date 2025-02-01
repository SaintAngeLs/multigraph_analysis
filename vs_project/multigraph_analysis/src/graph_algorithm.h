#ifndef GRAPH_ALGORITHM_H
#define GRAPH_ALGORITHM_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

#include "graph_interface.h"
#include "approximation_algorithm.h"
#include "utils.h"

/* Threshold to switch between exact and approximate algorithms */
#define THRESHOLD 10

/*
 * GraphAlgorithmContext
 * ---------------------
 * A struct containing a pointer to a GraphInterface + the number of vertices.
 */
typedef struct GraphAlgorithmContext {
    GraphInterface* graph_interface;
    int vertices;
} GraphAlgorithmContext;

/* Create/destroy contexts for convenience */
GraphAlgorithmContext* create_context(void* graph, int vertices);
void destroy_context(GraphAlgorithmContext* context);

/*
 * GraphAlgorithm
 * --------------
 * A set of function pointers implementing various graph operations:
 *   - calculate_size
 *   - find_cycles, count_hamiltonian_cycles
 *   - calculate_metric
 *   - find_minimal_extension
 *   - count_maximal_cycles, find_maximal_cycles
 */
typedef struct {
    /* Returns a size measure (e.g., number of edges) */
    int (*calculate_size)(void* graph);

    /*
     * find_cycles
     * Finds all (simple) cycles in the graph. If the graph is large, calls the approximation.
     * Outputs:
     *   - output_cycles: (int***) array-of-arrays for the cycles found
     *   - cycle_sizes:   (int*)   lengths for each cycle
     *   - cycle_count:   total # of cycles
     */
    int (*find_cycles)(
        void* graph,
        int vertices,
        int*** output_cycles,
        int** cycle_sizes,
        int* cycle_count
        );

    /*
     * count_hamiltonian_cycles
     * Finds all Hamiltonian cycles (or approximates if large).
     */
    int (*count_hamiltonian_cycles)(
        void* graph,
        int vertices,
        int*** output_cycles,
        int** cycle_sizes,
        int* cycle_count
        );

    /*
     * calculate_metric
     * Calculates a "distance" or "metric" between two graphs, storing results
     * in exact_metric and approximate_metric.
     */
    void (*calculate_metric)(
        void* graph_1, int vertices_1,
        void* graph_2, int vertices_2,
        int* exact_metric,
        int* approximate_metric
        );

    /*
     * find_minimal_extension
     * Minimal # of edges to add so the graph has a Hamiltonian cycle.
     */
    int (*find_minimal_extension)(
        void* graph,
        int vertices
        );

    /*
     * count_maximal_cycles
     * Returns the length of the longest simple cycle in the graph.
     */
    int (*count_maximal_cycles)(
        void* graph,
        int vertices
        );

    /*
     * find_maximal_cycles
     * Finds all cycles whose length = the max cycle length.
     * Output is in triple-pointer-of-int + sizes array + cycle_count.
     */
    int (*find_maximal_cycles)(
        void* graph,
        int vertices,
        int*** output_cycles,
        int** cycle_sizes,
        int* cycle_count
        );

} GraphAlgorithm;

/* The global default_algorithm object with pointers to implementations */
extern GraphAlgorithm default_algorithm;

/*-----------------------------------------------------------------------------
 * Additional Data Structures & Helper Function Declarations
 *  (exposed so you can see or call them if desired)
 *---------------------------------------------------------------------------*/

 /*
  * A dynamic array to collect integer-arrays (cycles) & their lengths.
  */
typedef struct {
    int** cycles;
    int* sizes;
    int   count;
    int   capacity;
} CycleList;

/*
 * A dynamic array of strings for storing unique cycle representations.
 */
typedef struct {
    char** data;
    int   count;
    int   capacity;
} StringList;

/* Initialize / free / add-to CycleList */
void initCycleList(CycleList* cl);
void freeCycleList(CycleList* cl);
void addCycle(CycleList* cl, const int* cycle, int length);

/* Initialize / free / check / add to StringList */
void initStringList(StringList* sl);
void freeStringList(StringList* sl);
bool stringListContains(const StringList* sl, const char* str);
void addStringToList(StringList* sl, const char* str);

/* Produces a canonical string representation for a cycle (smallest rotation) */
char* normalizeCycle(const int* array, int length);

/* Standard "next_permutation"-like in C */
bool nextPermutation(int* arr, int n);

/* Calculate required operations for a given permutation (used in graph metric) */
int calculate_required_operations(
    GraphAlgorithmContext* context1,
    GraphAlgorithmContext* context2,
    int* arr,
    int n,
    int smaller_n
);

/* The exact graph metric by enumerating permutations (somewhat expensive) */
int calculate_graph_metric(
    GraphAlgorithmContext* context1,
    GraphAlgorithmContext* context2,
    int vertices1,
    int vertices2
);

/* DFS for non-Hamiltonian cycles */
void dfs_findCycles(
    GraphAlgorithmContext* context,
    int start,
    int current,
    int* stack,
    int stackLen,
    bool* visited,
    StringList* uniqueCycles,
    CycleList* outputList,
    int* cycleCount
);

/* DFS for Hamiltonian cycles */
void backtrack_hamiltonian(
    GraphAlgorithmContext* context,
    int start,
    int current,
    int depth,
    int* path,
    bool* visited,
    StringList* uniqueCycles,
    CycleList* outputList,
    int* count
);

/* Check if there's at least one Hamiltonian cycle */
bool hasHamiltonianCycle(GraphAlgorithmContext* context);

/* Used by hasHamiltonianCycle to do a DFS check */
void checkCycle(
    GraphAlgorithmContext* context,
    int start,
    int current,
    int depth,
    int* path,
    bool* visited,
    bool* foundOne
);

/* DFS for length of the longest cycle */
void dfs_maxCycleLength(
    GraphAlgorithmContext* context,
    int start,
    int current,
    int depth,
    bool* visited,
    int* maxLen
);

/* DFS to find all cycles that match the maximum length */
void dfs_findMaxCycles(
    GraphAlgorithmContext* context,
    int start,
    int current,
    int* stack,
    int stackLen,
    bool* visited,
    StringList* uniqueCycles,
    CycleList* outputList,
    int* maxCycleLen,
    int* foundCount
);

/* Explores adding edges for minimal extension until a Hamiltonian cycle is formed */
void explore_extensions(
    GraphAlgorithmContext* context,
    int vertices,
    int addedEdges,
    int* minEdgesNeeded
);

#endif /* GRAPH_ALGORITHM_H */
