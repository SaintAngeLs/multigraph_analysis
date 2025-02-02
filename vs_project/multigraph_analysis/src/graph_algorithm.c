
#include "graph_algorithm.h"
#include "common_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Function to normalize cycle representation */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Function to normalize cycle representation */
char* normalizeCycle(int* cycle, int length) {
    if (length <= 0) return NULL;

    int minIndex = 0;
    for (int i = 1; i < length; i++) {
        if (cycle[i] < cycle[minIndex]) {
            minIndex = i;
        }
    }

    // Allocate enough space for a unique cycle representation
    char* forward = (char*)malloc(256);
    char* reverse = (char*)malloc(256);
    forward[0] = '\0';
    reverse[0] = '\0';

    // Generate the canonical representation (forward order)
    for (int i = 0; i < length; i++) {
        char buffer[12];
        sprintf(buffer, "%d-", cycle[(minIndex + i) % length]);
        strcat(forward, buffer);
    }

    // Generate the reverse canonical representation (backward order)
    for (int i = 0; i < length; i++) {
        char buffer[12];
        sprintf(buffer, "%d-", cycle[(minIndex - i + length) % length]);
        strcat(reverse, buffer);
    }

    // Choose the lexicographically smaller representation
    char* canonical = (strcmp(forward, reverse) < 0) ? forward : reverse;

    char* result = (char*)malloc(strlen(canonical) + 1);
    strcpy(result, canonical);

    free(forward);
    free(reverse);

    return result;
}






/* nextPermutation for exact metric calculation */
bool nextPermutation(int* arr, int n) {
    int i = n - 2;
    while (i >= 0 && arr[i] >= arr[i + 1]) i--;
    if (i < 0) return false;
    int j = n - 1;
    while (arr[j] <= arr[i]) j--;
    common_swap(&arr[i], &arr[j]);
    int left = i + 1, right = n - 1;
    while (left < right) {
        common_swap(&arr[left], &arr[right]);
        left++;
        right--;
    }
    return true;
}

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
) {
    push(stack, current);
    visited[current] = true;

    GraphInterface* gi = ctx->graph_interface;
    int stackLen = stack->top + 1;

    int out_degree;
    int* neighbors = gi->get_all_edges_from_vertex(gi, current, &out_degree);
    /*printf("Node %d has %d neighbors: ", current, out_degree);
    for (int i = 0; i < out_degree; i++) {
        printf("%d ", neighbors[i]);
    }
    printf("\n");*/

    for (int i = 0; i < out_degree; i++) {
        int nxt = neighbors[i];

        if (nxt == start && stackLen >= 2) {
            // **Valid cycle found**
            int cycleLength = stackLen + 1;
            int* cycleArray = (int*)malloc(cycleLength * sizeof(int));
            for (int j = 0; j < stackLen; j++) {
                cycleArray[j] = stack->data[j];
            }
            cycleArray[cycleLength - 1] = start; // **Close the cycle**

            /* printf("[Vefore CYCLE FOUND]: ");
            for (int j = 0; j < cycleLength; j++) {
                printf("%d ", cycleArray[j]);
            }
            printf("\n");*/
            char* canonical = normalizeCycle(cycleArray, cycleLength);
            if (!stringListContains(uniqueCycles, canonical)) {
                /* printf("[CYCLE FOUND]: ");
                 for (int j = 0; j < cycleLength; j++) {
                     printf("%d ", cycleArray[j]);
                 }
                 printf("\n");*/

                addStringToList(uniqueCycles, canonical);
                addCycle(cycleList, cycleArray, cycleLength);
                (*localCount)++;
            }

            free(canonical);
            free(cycleArray);
        }
        else if (!visited[nxt]) {
            dfs_findCycles(ctx, start, nxt, stack, visited, uniqueCycles, cycleList, localCount);
        }
    }

    pop(stack);
    visited[current] = false;
    free(neighbors);
}



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
)
{
    push(stack, current);
    visited[current] = true;

    GraphInterface* gi = context->graph_interface;
    int depth = stack->top + 1;  // how many distinct vertices visited so far

    if (depth == context->vertices) {
        // Try to close the cycle back to 'start'
        int w = gi->get_edge(gi, current, start);
        if (w > 0) {
            int cycleLength = depth + 1;
            int* cycleArray = (int*)malloc(cycleLength * sizeof(int));
            for (int i = 0; i < depth; i++) {
                cycleArray[i] = stack->data[i];
            }
            cycleArray[depth] = start; // close cycle

            char* canonical = normalizeCycle(cycleArray, cycleLength);
            if (!stringListContains(uniqueCycles, canonical)) {
                addStringToList(uniqueCycles, canonical);
                addCycle(cycleList, cycleArray, cycleLength);
                (*localCount)++;
            }
            free(canonical);
            free(cycleArray);
        }
    }
    else {
        // Try all possible next vertices that aren't visited
        for (int nxt = 0; nxt < context->vertices; nxt++) {
            int w2 = gi->get_edge(gi, current, nxt);
            if (w2 > 0 && !visited[nxt]) {
                backtrack_hamiltonian(context, start, nxt, stack,
                    visited, uniqueCycles, cycleList, localCount);
            }
        }
    }

    // Backtrack
    pop(stack);
    visited[current] = false;
}

/*
 * (3) Maximal cycle length: we only need a "depth" count. The maximum
 *     simple cycle length found across any DFS is stored in *maxLen.
 */
void dfs_maxCycleLength(
    GraphAlgorithmContext* context,
    int start,
    int current,
    Stack* stack,
    bool* visited,
    int* maxLen
)
{
    push(stack, current);
    visited[current] = true;
    int depth = stack->top + 1;

    GraphInterface* gi = context->graph_interface;
    for (int nxt = 0; nxt < context->vertices; nxt++) {
        int w = gi->get_edge(gi, current, nxt);
        if (w > 0) {
            // If we returned to 'start', we have a cycle. The length is depth+1.
            if (nxt == start && depth >= 1) {
                if (depth > *maxLen) {
                    *maxLen = depth;
                }
            }
            else if (!visited[nxt]) {
                dfs_maxCycleLength(context, start, nxt, stack, visited, maxLen);
            }
        }
    }
    // backtrack
    pop(stack);
    visited[current] = false;
}

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
)
{
    push(stack, current);
    visited[current] = true;
    int stackLen = stack->top + 1;

    GraphInterface* gi = context->graph_interface;
    for (int nxt = 0; nxt < context->vertices; nxt++) {
        int w = gi->get_edge(gi, current, nxt);
        if (w > 0) {
            // If we closed a cycle of length == *maxCycleLen, record it
            if (nxt == start && stackLen == *maxCycleLen) {
                int cycleLength = stackLen + 1;  // e.g. 3 -> 4 array
                int* cycleArray = (int*)malloc(cycleLength * sizeof(int));
                for (int i = 0; i < stackLen; i++) {
                    cycleArray[i] = stack->data[i];
                }
                cycleArray[stackLen] = start;

                char* canonical = normalizeCycle(cycleArray, cycleLength);
                if (!stringListContains(uniqueCycles, canonical)) {
                    addStringToList(uniqueCycles, canonical);
                    addCycle(outputList, cycleArray, cycleLength);
                    (*foundCount)++;
                }
                free(canonical);
                free(cycleArray);
            }
            else if (!visited[nxt] && stackLen < *maxCycleLen) {
                dfs_findMaxCycles(context, start, nxt, stack, visited,
                    uniqueCycles, outputList, maxCycleLen, foundCount);
            }
        }
    }
    pop(stack);
    visited[current] = false;
}

int calculate_graph_metric(
    GraphAlgorithmContext* context1,
    GraphAlgorithmContext* context2,
    int vertices1,
    int vertices2
)
{
    if (vertices1 > THRESHOLD || vertices2 > THRESHOLD) {
        printf("Using approximate algorithm for large graphs.\n");
        return approximate_calculate_metric(context1, context2, vertices1, vertices2);
    }

    int* arr = (int*)malloc(vertices1 * sizeof(int));
    for (int i = 0; i < vertices1; i++) {
        arr[i] = i + 1;
    }
    int minOperations = INT_MAX;
    do {
        int tmp = calculate_required_operations(context1, context2, arr, vertices1, vertices2);
        minOperations = common_min(minOperations, tmp);
    } while (nextPermutation(arr, vertices1));

    free(arr);
    return minOperations;
}

int calculate_required_operations(
    GraphAlgorithmContext* context1,
    GraphAlgorithmContext* context2,
    int* arr,
    int n,
    int smaller_n
)
{
    int required_operations = n - smaller_n;
    GraphInterface* g1 = context1->graph_interface;
    GraphInterface* g2 = context2->graph_interface;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i >= smaller_n || j >= smaller_n) {
                required_operations += g1->get_edge(g1, arr[i] - 1, arr[j] - 1);
            }
            else {
                int e1 = g1->get_edge(g1, arr[i] - 1, arr[j] - 1);
                int e2 = g2->get_edge(g2, i, j);
                required_operations += abs(e1 - e2);
            }
        }
    }
    return required_operations;
}

void explore_extensions(GraphAlgorithmContext* context, int vertices, int addedEdges, int* minEdgesNeeded) {
    if (addedEdges >= *minEdgesNeeded) return;
    if (hasHamiltonianCycle(context)) {
        if (addedEdges < *minEdgesNeeded) {
            *minEdgesNeeded = addedEdges;
        }
        return;
    }
    GraphInterface* gi = context->graph_interface;
 
    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            if (i != j) {
                int w = gi->get_edge(gi, i, j);
                if (w == 0) {
                    gi->add_edge(gi, i, j, 1);
                    explore_extensions(context, vertices, addedEdges + 1, minEdgesNeeded);
                    gi->add_edge(gi, i, j, -1);
                }
            }
        }
    }
}

bool hasHamiltonianCycle(GraphAlgorithmContext* context) {
    bool foundOne = false;
    for (int i = 0; i < context->vertices && !foundOne; i++) {
        int* path = (int*)malloc(context->vertices * sizeof(int));
        bool* visited = (bool*)calloc(context->vertices, sizeof(bool));
        if (!path || !visited) {
            free(path);
            free(visited);
            continue;
        }
        checkCycle(context, i, i, 1, path, visited, &foundOne);
        free(path);
        free(visited);
    }
    return foundOne;
}

void checkCycle(GraphAlgorithmContext* context, int start, int current, int depth,
    int* path, bool* visited, bool* foundOne)
{
    if (*foundOne) return;
    path[depth - 1] = current;
    visited[current] = true;

    GraphInterface* gi = context->graph_interface;
    if (depth == context->vertices) {
        int w = gi->get_edge(gi, current, start);
        if (w > 0) {
            *foundOne = true;
        }
    }
    else {
        for (int nxt = 0; nxt < context->vertices && !(*foundOne); nxt++) {
            int w2 = gi->get_edge(gi, current, nxt);
            if (w2 > 0 && !visited[nxt]) {
                checkCycle(context, start, nxt, depth + 1, path, visited, foundOne);
            }
        }
    }
    visited[current] = false;
}