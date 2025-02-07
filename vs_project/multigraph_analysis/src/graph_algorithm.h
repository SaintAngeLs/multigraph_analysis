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
#include "stack.h"
#include "string_list.h"
#include "cycle_list.h"
#include "graph_algorithm_context.h"

#define THRESHOLD 10


char* normalizeCycle(int* cycle, int length);


/* nextPermutation for exact metric calculation */
bool nextPermutation(int* arr, int n);
/*-----------------------------------------------------------------------------
 * EXACT DFS routines
 *---------------------------------------------------------------------------*/

 /*
  * (1) Non-Hamiltonian cycles: all simple cycles (no repeated vertices),
  *     of length >= 2 edges.
  */
void dfs_findCycles(
    GraphAlgorithmContext* ctx,
    int start,
    int current,
    Stack* stack,
    bool* visited,
    StringList* uniqueCycles,
    CycleList* cycleList,
    int* localCount
);


/*
 * (2) Hamiltonian cycles (backtrack).
 *     We'll also keep the path in a stack for clarity.
 */
void backtrack_hamiltonian(
    GraphAlgorithmContext* context,
    int start,
    int current,
    Stack* stack,
    bool* visited,
    StringList* uniqueCycles,
    CycleList* cycleList,
    int* localCount
);


void dfs_maxCycleLength(
    GraphAlgorithmContext* context,
    int start,
    int current,
    Stack* stack,
    bool* visited,
    int* maxLen
);

/*
 * (4) find all cycles exactly matching the maximum length *maxCycleLen
 */
void dfs_findMaxCycles(
    GraphAlgorithmContext* context,
    int start,
    int current,
    Stack* stack,
    bool* visited,
    StringList* uniqueCycles,
    CycleList* outputList,
    int* maxCycleLen,
    int* foundCount
);

int calculate_graph_metric(
    GraphAlgorithmContext* context1,
    GraphAlgorithmContext* context2,
    int vertices1,
    int vertices2
);

int calculate_required_operations(
    GraphAlgorithmContext* context1,
    GraphAlgorithmContext* context2,
    int* arr,
    int n,
    int smaller_n
);

void explore_extensions(GraphAlgorithmContext* context, int vertices, int addedEdges, int* minEdgesNeeded);

bool hasHamiltonianCycle(GraphAlgorithmContext* context);

void checkCycle(GraphAlgorithmContext* context, int start, int current, int depth,
    int* path, bool* visited, bool* foundOne);

#endif /* GRAPH_ALGORITHM_H */
