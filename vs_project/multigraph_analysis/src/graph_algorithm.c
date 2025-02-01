/******************************************************************************
 * File: graph_algorithm.c
 * Description: Implementation of graph algorithms in standard C (MSVC-friendly),
 *              no GLib usage, no nested functions.
 *
 * Dependencies:
 *   - graph_interface.h
 *   - approximation_algorithm.h
 *   - common_utils.h (for common_min, common_max, common_swap)
 *   - utils.h
 ******************************************************************************/

#include "graph_algorithm.h"
#include "common_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>

 /******************************************************************************
  * GraphAlgorithmContext creation and destruction
  *****************************************************************************/

GraphAlgorithmContext* create_context(void* graph, int vertices)
{
    GraphAlgorithmContext* context =
        (GraphAlgorithmContext*)malloc(sizeof(GraphAlgorithmContext));
    if (!context) {
        fprintf(stderr, "Error: failed to allocate GraphAlgorithmContext\n");
        return NULL;
    }
    context->graph_interface = (GraphInterface*)graph;
    context->vertices = vertices;
    return context;
}

void destroy_context(GraphAlgorithmContext* context)
{
    if (context) {
        free(context);
    }
}

/******************************************************************************
 * calculate_size - returns a "size" measure (e.g., # of edges)
 *****************************************************************************/
static int calculate_size(void* graph)
{
    if (!graph) {
        fprintf(stderr, "Error: Null graph pointer in calculate_size.\n");
        return -1;
    }
    GraphInterface* gi = (GraphInterface*)graph;
    if (!gi->calculate_size) {
        fprintf(stderr, "Error: GraphInterface missing calculate_size.\n");
        return -1;
    }
    return gi->calculate_size(graph);
}

/******************************************************************************
 * find_cycles_wrapper - enumerates all simple cycles if small, else approximate
 *****************************************************************************/
static int find_cycles_wrapper(
    void* graph,
    int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count
)
{
    if (vertices >= THRESHOLD) {
        /* For large graphs, use approximation */
        return approximate_find_cycles(graph, vertices, output_cycles, cycle_sizes, cycle_count);
    }

    GraphAlgorithmContext* context = create_context(graph, vertices);
    if (!context) return 0;

    CycleList cycleList;
    initCycleList(&cycleList);

    StringList uniqueCycles;
    initStringList(&uniqueCycles);

    int count = 0;

    /* For each vertex, do a DFS-based cycle search. */
    for (int start = 0; start < vertices; start++) {
        int* stack = (int*)malloc(vertices * sizeof(int));
        bool* visited = (bool*)calloc(vertices, sizeof(bool));
        if (!stack || !visited) {
            fprintf(stderr, "Memory allocation failure in find_cycles.\n");
            free(stack);
            free(visited);
            continue;
        }

        /* Use the declared dfs_findCycles from the header. */
        dfs_findCycles(context, start, start,
            stack, 0,
            visited,
            &uniqueCycles,
            &cycleList,
            &count);

        free(stack);
        free(visited);
    }

    /* Extract the results into user pointers */
    *output_cycles = cycleList.cycles;
    *cycle_sizes = cycleList.sizes;
    *cycle_count = cycleList.count;

    freeStringList(&uniqueCycles);
    destroy_context(context);

    return count;
}

/******************************************************************************
 * count_hamiltonian_cycles_wrapper - enumerates Hamiltonian cycles if small
 *****************************************************************************/
static int count_hamiltonian_cycles_wrapper(
    void* graph,
    int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count
)
{
    if (vertices >= THRESHOLD) {
        return approximate_count_hamiltonian_cycles(graph, vertices,
            output_cycles,
            cycle_sizes,
            cycle_count);
    }

    GraphAlgorithmContext* context = create_context(graph, vertices);
    if (!context) return 0;

    CycleList cycleList;
    initCycleList(&cycleList);

    StringList uniqueCycles;
    initStringList(&uniqueCycles);

    int count = 0;

    /* Try each vertex as a start node of a Hamiltonian path. */
    for (int start = 0; start < vertices; start++) {
        int* path = (int*)malloc(vertices * sizeof(int));
        bool* visited = (bool*)calloc(vertices, sizeof(bool));
        if (!path || !visited) {
            free(path);
            free(visited);
            continue;
        }

        /* Use the declared backtrack_hamiltonian from the header. */
        backtrack_hamiltonian(context,
            start, start, 1,
            path, visited,
            &uniqueCycles, &cycleList, &count);

        free(path);
        free(visited);
    }

    *output_cycles = cycleList.cycles;
    *cycle_sizes = cycleList.sizes;
    *cycle_count = cycleList.count;

    freeStringList(&uniqueCycles);
    destroy_context(context);

    return count;
}

/******************************************************************************
 * calculate_metric_wrapper - measure difference between two graphs
 *****************************************************************************/
static void calculate_metric_wrapper(
    void* graph1, int vertices1,
    void* graph2, int vertices2,
    int* exact_metric,
    int* approximate_metric
)
{
    if (common_max(vertices1, vertices2) == vertices2) {
        /* Swap so that graph1 always has the max # of vertices */
        void* tmp = graph1;
        graph1 = graph2;
        graph2 = tmp;

        int tmpv = vertices1;
        vertices1 = vertices2;
        vertices2 = tmpv;
    }

    GraphAlgorithmContext* context1 = create_context(graph1, vertices1);
    GraphAlgorithmContext* context2 = create_context(graph2, vertices2);
    if (!context1 || !context2) {
        if (context1) destroy_context(context1);
        if (context2) destroy_context(context2);
        return;
    }

    clock_t start = clock();
    /* The user-exposed function declared in the header. */
    *exact_metric = calculate_graph_metric(context1, context2, vertices1, vertices2);
    clock_t end = clock();
    double exactTime = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time taken for exact metric: %f seconds\n", exactTime);

    start = clock();
    *approximate_metric = approximate_calculate_metric(context1, context2, vertices1, vertices2);
    end = clock();
    double approxTime = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time taken for approximate metric: %f seconds\n", approxTime);

    destroy_context(context1);
    destroy_context(context2);
}

/******************************************************************************
 * find_minimal_extension_wrapper - minimal edges for Hamiltonian cycle
 *****************************************************************************/
static int find_minimal_extension_wrapper(void* graph, int vertices)
{
    if (vertices >= THRESHOLD) {
        return approximate_find_minimal_extension(graph, vertices);
    }

    GraphAlgorithmContext* context = create_context(graph, vertices);
    if (!context) return 0;

    int minEdgesNeeded = INT_MAX;

    /* Quick check if we already have a Hamiltonian cycle */
    bool hasHam = hasHamiltonianCycle(context);
    if (hasHam) {
        destroy_context(context);
        return 0;
    }

    /* We'll do naive DFS exploring ways to add edges. */
    explore_extensions(context, vertices, 0, &minEdgesNeeded);

    destroy_context(context);
    if (minEdgesNeeded == INT_MAX) return 0; /* Means we never found a cycle. */
    return minEdgesNeeded;
}

/******************************************************************************
 * count_maximal_cycles_wrapper - length of the longest simple cycle
 *****************************************************************************/
static int count_maximal_cycles_wrapper(void* graph, int vertices)
{
    if (vertices >= THRESHOLD) {
        return approximate_count_maximal_cycles(graph, vertices);
    }

    GraphAlgorithmContext* context = create_context(graph, vertices);
    if (!context) return 0;

    int maxCycleLength = 0;
    for (int start = 0; start < vertices; start++) {
        bool* visited = (bool*)calloc(vertices, sizeof(bool));
        if (!visited) continue;
        dfs_maxCycleLength(context, start, start, 1, visited, &maxCycleLength);
        free(visited);
    }

    destroy_context(context);
    return maxCycleLength;
}

/******************************************************************************
 * find_maximal_cycles_wrapper - enumerates all cycles of max length
 *****************************************************************************/
static int find_maximal_cycles_wrapper(
    void* graph,
    int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count
)
{
    if (vertices >= THRESHOLD) {
        return approximate_find_maximal_cycles(graph, vertices,
            output_cycles,
            cycle_sizes,
            cycle_count);
    }

    GraphAlgorithmContext* context = create_context(graph, vertices);
    if (!context) {
        *output_cycles = NULL;
        *cycle_sizes = NULL;
        *cycle_count = 0;
        return 0;
    }

    /* Find maximum cycle length first */
    int maxCycleLength = 0;
    for (int start = 0; start < vertices; start++) {
        bool* visited = (bool*)calloc(vertices, sizeof(bool));
        if (!visited) continue;
        dfs_maxCycleLength(context, start, start, 1, visited, &maxCycleLength);
        free(visited);
    }

    /* Collect all cycles that match that length */
    CycleList cycleList;
    initCycleList(&cycleList);

    StringList uniqueCycles;
    initStringList(&uniqueCycles);

    int foundCount = 0;

    for (int start = 0; start < vertices; start++) {
        int* stack = (int*)malloc(vertices * sizeof(int));
        bool* visited = (bool*)calloc(vertices, sizeof(bool));
        if (!stack || !visited) {
            free(stack);
            free(visited);
            continue;
        }

        dfs_findMaxCycles(context, start, start,
            stack, 0, visited,
            &uniqueCycles, &cycleList,
            &maxCycleLength, &foundCount);

        free(stack);
        free(visited);
    }

    *output_cycles = cycleList.cycles;
    *cycle_sizes = cycleList.sizes;
    *cycle_count = cycleList.count;

    freeStringList(&uniqueCycles);
    destroy_context(context);

    return foundCount;
}

/******************************************************************************
 * Our exported default_algorithm structure
 *****************************************************************************/
GraphAlgorithm default_algorithm = {
    .calculate_size = calculate_size,
    .find_cycles = find_cycles_wrapper,
    .count_hamiltonian_cycles = count_hamiltonian_cycles_wrapper,
    .calculate_metric = calculate_metric_wrapper,
    .find_minimal_extension = find_minimal_extension_wrapper,
    .count_maximal_cycles = count_maximal_cycles_wrapper,
    .find_maximal_cycles = find_maximal_cycles_wrapper
};

/******************************************************************************
 *          IMPLEMENTATIONS OF EXPOSED HELPER FUNCTIONS
 *          (Now declared in graph_algorithm.h)
 *****************************************************************************/

 /*================= CycleList Implementation =================*/
void initCycleList(CycleList* cl)
{
    cl->count = 0;
    cl->capacity = INITIAL_CYCLELIST_CAPACITY;
    cl->cycles = (int**)malloc(cl->capacity * sizeof(int*));
    cl->sizes = (int*)malloc(cl->capacity * sizeof(int));
}

void freeCycleList(CycleList* cl)
{
    if (!cl) return;
    for (int i = 0; i < cl->count; i++) {
        free(cl->cycles[i]);
    }
    free(cl->cycles);
    free(cl->sizes);
    cl->count = 0;
    cl->capacity = 0;
}

void addCycle(CycleList* cl, const int* cycle, int length)
{
    if (cl->count == cl->capacity) {
        cl->capacity *= 2;
        cl->cycles = (int**)realloc(cl->cycles, cl->capacity * sizeof(int*));
        cl->sizes = (int*)realloc(cl->sizes, cl->capacity * sizeof(int));
    }
    if (!cl->cycles || !cl->sizes) {
        fprintf(stderr, "Memory reallocation failed in addCycle.\n");
        return;
    }
    int* newCycle = (int*)malloc(length * sizeof(int));
    memcpy(newCycle, cycle, length * sizeof(int));

    cl->cycles[cl->count] = newCycle;
    cl->sizes[cl->count] = length;
    cl->count++;
}

/*================= StringList Implementation =================*/

void initStringList(StringList* sl)
{
    sl->count = 0;
    sl->capacity = INITIAL_STRINGLIST_CAPACITY;
    sl->data = (char**)malloc(sl->capacity * sizeof(char*));
}

void freeStringList(StringList* sl)
{
    if (!sl) return;
    for (int i = 0; i < sl->count; i++) {
        free(sl->data[i]);
    }
    free(sl->data);
    sl->count = 0;
    sl->capacity = 0;
}

bool stringListContains(const StringList* sl, const char* str)
{
    for (int i = 0; i < sl->count; i++) {
        if (strcmp(sl->data[i], str) == 0) {
            return true;
        }
    }
    return false;
}

void addStringToList(StringList* sl, const char* str)
{
    if (sl->count == sl->capacity) {
        sl->capacity *= 2;
        sl->data = (char**)realloc(sl->data, sl->capacity * sizeof(char*));
    }
    if (!sl->data) {
        fprintf(stderr, "Memory reallocation failed in addStringToList.\n");
        return;
    }
    sl->data[sl->count] = (char*)malloc(strlen(str) + 1);
    strcpy(sl->data[sl->count], str);
    sl->count++;
}

/******************************************************************************
 * normalizeCycle - produce a canonical string (smallest rotation)
 *****************************************************************************/
char* normalizeCycle(const int* array, int length)
{
    /* Copy cycle into an extended array of length*2 to compare rotations */
    int* extended = (int*)malloc(length * 2 * sizeof(int));
    for (int i = 0; i < length; i++) {
        extended[i] = array[i];
        extended[i + length] = array[i];
    }

    /* Find the rotation that is lexicographically smallest */
    int minIndex = 0;
    for (int start = 1; start < length; start++) {
        for (int k = 0; k < length; k++) {
            if (extended[start + k] < extended[minIndex + k]) {
                minIndex = start;
                break;
            }
            else if (extended[start + k] > extended[minIndex + k]) {
                break;
            }
        }
    }

    /* Build a string like "1-3-5-" */
    int bufferSize = length * 12 + 1; /* up to ~12 chars per element */
    char* result = (char*)malloc(bufferSize);
    result[0] = '\0';

    for (int i = 0; i < length; i++) {
        char temp[32];
        sprintf(temp, "%d-", extended[minIndex + i]);
        strcat(result, temp);
    }

    free(extended);
    return result;
}

/******************************************************************************
 * nextPermutation - standard "C++-like" in plain C
 *****************************************************************************/
bool nextPermutation(int* arr, int n)
{
    int i = n - 2;
    while (i >= 0 && arr[i] >= arr[i + 1]) {
        i--;
    }
    if (i < 0) {
        return false;
    }

    int j = n - 1;
    while (arr[j] <= arr[i]) {
        j--;
    }

    common_swap(&arr[i], &arr[j]);

    int left = i + 1;
    int right = n - 1;
    while (left < right) {
        common_swap(&arr[left], &arr[right]);
        left++;
        right--;
    }

    return true;
}

/******************************************************************************
 * calculate_required_operations - used by calculate_graph_metric
 *****************************************************************************/
int calculate_required_operations(
    GraphAlgorithmContext* context1,
    GraphAlgorithmContext* context2,
    int* arr,
    int n,
    int smaller_n
)
{
    int required_operations = n - smaller_n; /* difference in # of vertices */

    GraphInterface* g1 = context1->graph_interface;
    GraphInterface* g2 = context2->graph_interface;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i >= smaller_n || j >= smaller_n) {
                /* outside smaller graph's range => difference */
                required_operations += g1->get_edge(g1, arr[i] - 1, arr[j] - 1);
            }
            else {
                /* compare edges in both graphs */
                int e1 = g1->get_edge(g1, arr[i] - 1, arr[j] - 1);
                int e2 = g2->get_edge(g2, i, j);
                required_operations += abs(e1 - e2);
            }
        }
    }

    return required_operations;
}

/******************************************************************************
 * calculate_graph_metric - exact approach by enumerating permutations
 *****************************************************************************/
int calculate_graph_metric(
    GraphAlgorithmContext* context1,
    GraphAlgorithmContext* context2,
    int vertices1,
    int vertices2
)
{
    /* If large, fallback to approximation. */
    if (vertices1 > THRESHOLD || vertices2 > THRESHOLD) {
        printf("Using approximate algorithm for large graphs.\n");
        return approximate_calculate_metric(context1, context2, vertices1, vertices2);
    }

    /* Build array [1..vertices1] for permutations. */
    int* arr = (int*)malloc(vertices1 * sizeof(int));
    if (!arr) {
        fprintf(stderr, "Memory allocation error in calculate_graph_metric.\n");
        return INT_MAX;
    }
    for (int i = 0; i < vertices1; i++) {
        arr[i] = i + 1;
    }

    int minOperations = INT_MAX;

    /* Enumerate all permutations of arr. */
    do {
        int tmp = calculate_required_operations(context1, context2, arr, vertices1, vertices2);
        minOperations = common_min(minOperations, tmp);
    } while (nextPermutation(arr, vertices1));

    free(arr);
    return minOperations;
}

/******************************************************************************
 * dfs_findCycles - DFS to find all non-Hamiltonian cycles
 *****************************************************************************/
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
)
{
    stack[stackLen] = current;
    visited[current] = true;

    GraphInterface* gi = context->graph_interface;

    for (int nxt = 0; nxt < context->vertices; nxt++) {
        int w = gi->get_edge(gi, current, nxt);
        if (w > 0) {
            if (nxt == start && (w > 1 || stackLen > 1)) {
                /* Found a cycle */
                char* canonical = normalizeCycle(stack, stackLen + 1);
                if (!stringListContains(uniqueCycles, canonical)) {
                    addStringToList(uniqueCycles, canonical);
                    addCycle(outputList, stack, stackLen + 1);
                    (*cycleCount)++;
                }
                free(canonical);
            }
            else if (!visited[nxt]) {
                dfs_findCycles(context, start, nxt,
                    stack, stackLen + 1,
                    visited, uniqueCycles,
                    outputList, cycleCount);
            }
        }
    }

    visited[current] = false;
}

/******************************************************************************
 * backtrack_hamiltonian - enumerates Hamiltonian cycles
 *****************************************************************************/
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
)
{
    path[depth - 1] = current;
    visited[current] = true;

    GraphInterface* gi = context->graph_interface;

    if (depth == context->vertices) {
        /* Attempt to close cycle back to 'start' */
        int w = gi->get_edge(gi, current, start);
        if (w > 0 && (w > 1 || depth > 2)) {
            /* Found a Hamiltonian cycle */
            char* canonical = normalizeCycle(path, depth);
            if (!stringListContains(uniqueCycles, canonical)) {
                addStringToList(uniqueCycles, canonical);
                addCycle(outputList, path, depth);
                (*count)++;
            }
            free(canonical);
        }
    }
    else {
        for (int nxt = 0; nxt < context->vertices; nxt++) {
            int w2 = gi->get_edge(gi, current, nxt);
            if (w2 > 0 && !visited[nxt]) {
                backtrack_hamiltonian(context, start, nxt,
                    depth + 1, path,
                    visited, uniqueCycles,
                    outputList, count);
            }
        }
    }

    visited[current] = false;
}

/******************************************************************************
 * hasHamiltonianCycle - checks if at least one Hamiltonian cycle exists
 *****************************************************************************/
bool hasHamiltonianCycle(GraphAlgorithmContext* context)
{
    bool foundOne = false;

    /* Try each vertex as start. If foundOne becomes true, break. */
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

/*
 * checkCycle - helper for hasHamiltonianCycle
 */
void checkCycle(
    GraphAlgorithmContext* context,
    int start,
    int current,
    int depth,
    int* path,
    bool* visited,
    bool* foundOne
)
{
    if (*foundOne) return;

    path[depth - 1] = current;
    visited[current] = true;

    GraphInterface* gi = context->graph_interface;

    if (depth == context->vertices) {
        int w = gi->get_edge(gi, current, start);
        if (w > 0 && (w > 1 || depth > 2)) {
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

/******************************************************************************
 * dfs_maxCycleLength - DFS to find the longest cycle from "start"
 *****************************************************************************/
void dfs_maxCycleLength(
    GraphAlgorithmContext* context,
    int start,
    int current,
    int depth,
    bool* visited,
    int* maxLen
)
{
    visited[current] = true;

    GraphInterface* gi = context->graph_interface;
    for (int nxt = 0; nxt < context->vertices; nxt++) {
        int w = gi->get_edge(gi, current, nxt);
        if (w > 0) {
            if (nxt == start && (w > 1 || depth > 1)) {
                if (depth > *maxLen) {
                    *maxLen = depth;
                }
            }
            else if (!visited[nxt]) {
                dfs_maxCycleLength(context, start, nxt, depth + 1, visited, maxLen);
            }
        }
    }

    visited[current] = false;
}

/******************************************************************************
 * dfs_findMaxCycles - enumerates cycles exactly matching "maxCycleLen"
 *****************************************************************************/
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
)
{
    stack[stackLen] = current;
    visited[current] = true;

    GraphInterface* gi = context->graph_interface;

    for (int nxt = 0; nxt < context->vertices; nxt++) {
        int w = gi->get_edge(gi, current, nxt);
        if (w > 0) {
            if (nxt == start && (w > 1 || stackLen > 0)) {
                int cycleLength = stackLen + 1;
                if (cycleLength == *maxCycleLen) {
                    char* canonical = normalizeCycle(stack, cycleLength);
                    if (!stringListContains(uniqueCycles, canonical)) {
                        addStringToList(uniqueCycles, canonical);
                        addCycle(outputList, stack, cycleLength);
                        (*foundCount)++;
                    }
                    free(canonical);
                }
            }
            else if (!visited[nxt]) {
                dfs_findMaxCycles(context,
                    start, nxt,
                    stack, stackLen + 1,
                    visited,
                    uniqueCycles,
                    outputList,
                    maxCycleLen,
                    foundCount);
            }
        }
    }

    visited[current] = false;
}

/******************************************************************************
 * explore_extensions - tries adding edges to get a Hamiltonian cycle
 *****************************************************************************/
void explore_extensions(
    GraphAlgorithmContext* context,
    int vertices,
    int addedEdges,
    int* minEdgesNeeded
)
{
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
                    gi->add_edge(gi, i, j, -1); /* remove the edge */
                }
            }
        }
    }
}
