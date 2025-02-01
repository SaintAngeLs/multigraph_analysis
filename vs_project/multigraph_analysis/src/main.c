#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#define NOMINMAX
#include <windows.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>

#include "graph.h"
#include "utils.h"
#include "../include/config.h"
#include "graph_interface.h"
#include "common_utils.h"

 /*-----------------------------------------------------------------------------
  * Macros & Constants
  *---------------------------------------------------------------------------*/
#define THRESHOLD     10      /* If vertices >= THRESHOLD, use approximate */
#define MAX_ITER      20000   /* For approximate sim-anneal logic */
#define INITIAL_TEMP  200.0
#define COOLING_RATE  0.99

  /*-----------------------------------------------------------------------------
   * GraphAlgorithmContext
   *  - a simple struct to hold (GraphInterface, #vertices)
   *---------------------------------------------------------------------------*/
typedef struct {
    GraphInterface* graph_interface;
    int vertices;
} GraphAlgorithmContext;

/* Create/destroy context */
static GraphAlgorithmContext* create_context(void* graph, int vertices) {
    GraphAlgorithmContext* ctx = (GraphAlgorithmContext*)malloc(sizeof(GraphAlgorithmContext));
    if (!ctx) {
        fprintf(stderr, "Error: failed to allocate GraphAlgorithmContext\n");
        return NULL;
    }
    ctx->graph_interface = (GraphInterface*)graph;
    ctx->vertices = vertices;
    return ctx;
}
static void destroy_context(GraphAlgorithmContext* context) {
    if (context) free(context);
}

/*-----------------------------------------------------------------------------
 * CycleList: store arrays-of-int plus their lengths
 *---------------------------------------------------------------------------*/
typedef struct {
    int** cycles;
    int* sizes;
    int   count;
    int   capacity;
} CycleList;

#define INITIAL_CYCLELIST_CAPACITY 16

static void initCycleList(CycleList* cl) {
    cl->count = 0;
    cl->capacity = INITIAL_CYCLELIST_CAPACITY;
    cl->cycles = (int**)malloc(cl->capacity * sizeof(int*));
    cl->sizes = (int*)malloc(cl->capacity * sizeof(int));
}
static void freeCycleList(CycleList* cl) {
    if (!cl) return;
    for (int i = 0; i < cl->count; i++) {
        free(cl->cycles[i]);
    }
    free(cl->cycles);
    free(cl->sizes);
    cl->count = 0;
    cl->capacity = 0;
}
static void addCycle(CycleList* cl, const int* cycle, int length) {
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

/*-----------------------------------------------------------------------------
 * StringList: store unique string representations (avoid duplicates)
 *---------------------------------------------------------------------------*/
typedef struct {
    char** data;
    int   count;
    int   capacity;
} StringList;

#define INITIAL_STRINGLIST_CAPACITY 16

static void initStringList(StringList* sl) {
    sl->count = 0;
    sl->capacity = INITIAL_STRINGLIST_CAPACITY;
    sl->data = (char**)malloc(sl->capacity * sizeof(char*));
}
static void freeStringList(StringList* sl) {
    if (!sl) return;
    for (int i = 0; i < sl->count; i++) {
        free(sl->data[i]);
    }
    free(sl->data);
    sl->count = 0;
    sl->capacity = 0;
}
static bool stringListContains(const StringList* sl, const char* str) {
    for (int i = 0; i < sl->count; i++) {
        if (strcmp(sl->data[i], str) == 0) {
            return true;
        }
    }
    return false;
}
static void addStringToList(StringList* sl, const char* str) {
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

/*-----------------------------------------------------------------------------
 * Normalization of cycles & nextPermutation
 *---------------------------------------------------------------------------*/
static char* normalizeCycle(const int* array, int length) {
    /* Duplicate array => extended for rotation check */
    int* extended = (int*)malloc(length * 2 * sizeof(int));
    for (int i = 0; i < length; i++) {
        extended[i] = array[i];
        extended[i + length] = array[i];
    }
    /* Find lexicographically smallest rotation */
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
    /* Build string like "0-1-2-" */
    int bufferSize = length * 12 + 1;
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

static bool nextPermutation(int* arr, int n) {
    int i = n - 2;
    while (i >= 0 && arr[i] >= arr[i + 1]) {
        i--;
    }
    if (i < 0) return false;
    int j = n - 1;
    while (arr[j] <= arr[i]) {
        j--;
    }
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
 * EXACT DFS routines: findCycles, backtrack_hamiltonian, etc.
 *---------------------------------------------------------------------------*/

 /* 1) Non-Hamiltonian cycles */
static void dfs_findCycles(
    GraphAlgorithmContext* ctx,
    int start,
    int current,
    int* stack,
    int stackLen,
    bool* visited,
    StringList* uniqueCycles,
    CycleList* cycleList,
    int* localCount
)
{
    stack[stackLen] = current;
    visited[current] = true;
    GraphInterface* gi = ctx->graph_interface;

    for (int nxt = 0; nxt < ctx->vertices; nxt++) {
        int w = gi->get_edge(gi, current, nxt);
        if (w > 0) {
            if (nxt == start && (w > 1 || stackLen > 1)) {
                /* Found a cycle */
                int cycleLength = stackLen + 2;
                int* cycleArray = (int*)malloc(cycleLength * sizeof(int));
                memcpy(cycleArray, stack, (stackLen + 1) * sizeof(int));
                /* #34: explicitly repeat the start vertex  */
                cycleArray[stackLen + 1] = start; 

                /* Normalize for checking duplicates */
                char* canonical = normalizeCycle(cycleArray, cycleLength);

                if (!stringListContains(uniqueCycles, canonical)) {
                    addStringToList(uniqueCycles, canonical);
                    addCycle(cycleList, cycleArray, cycleLength);
                    (*localCount)++;
                }
                free(canonical);
                free(cycleArray);
            }
            else if (!visited[nxt]) {
                dfs_findCycles(ctx, start, nxt, stack, stackLen + 1,
                    visited, uniqueCycles, cycleList, localCount);
            }
        }
    }
    visited[current] = false;
}

/* 2) Hamiltonian cycles (backtrack) */
static void backtrack_hamiltonian(
    GraphAlgorithmContext* context,
    int start,
    int current,
    int depth,
    int* path,
    bool* visited,
    StringList* uniqueCycles,
    CycleList* cycleList,
    int* localCount
)
{
    path[depth - 1] = current;
    visited[current] = true;
    GraphInterface* gi = context->graph_interface;

    if (depth == context->vertices) {
        /* Attempt to close the cycle */
        int w = gi->get_edge(gi, current, start);
        /* #34: ----- CHANGED: no more (w > 1 || depth > 2).
         * Now if w>0, we do have a Hamiltonian cycle,
         * store the repeated start as well.
         */
        if (w > 0) {
            int cycleLength = depth + 1;
            int* cycleArray = (int*)malloc(cycleLength * sizeof(int));
            memcpy(cycleArray, path, depth * sizeof(int));
            cycleArray[depth] = start; /* close the cycle explicitly */

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
        for (int nxt = 0; nxt < context->vertices; nxt++) {
            int w2 = gi->get_edge(gi, current, nxt);
            if (w2 > 0 && !visited[nxt]) {
                backtrack_hamiltonian(context, start, nxt, depth + 1,
                    path, visited, uniqueCycles, cycleList, localCount);
            }
        }
    }
    visited[current] = false;
}

/* 3) Maximal cycles: enumerates all cycles that match the max length */
static void dfs_findMaxCycles(
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
            /* #34: ----- CHANGED: we just check if nxt == start and stackLen+1 = maxCycleLen */
            if (nxt == start && (stackLen + 1) == *maxCycleLen) {
                int cycleLength = stackLen + 2;
                int* cycleArray = (int*)malloc(cycleLength * sizeof(int));
                memcpy(cycleArray, stack, (stackLen + 1) * sizeof(int));
                cycleArray[stackLen + 1] = start;

                char* canonical = normalizeCycle(cycleArray, cycleLength);
                if (!stringListContains(uniqueCycles, canonical)) {
                    addStringToList(uniqueCycles, canonical);
                    addCycle(outputList, cycleArray, cycleLength);
                    (*foundCount)++;
                }
                free(canonical);
                free(cycleArray);
            }
            else if (!visited[nxt]) {
                dfs_findMaxCycles(context, start, nxt,
                    stack, stackLen + 1,
                    visited, uniqueCycles, outputList,
                    maxCycleLen, foundCount);
            }
        }
    }
    visited[current] = false;
}

/* 4) find the max cycle length (by DFS from each start) */
static void dfs_maxCycleLength(
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

/*-----------------------------------------------------------------------------
 * Approximate logic (for large graphs)
 *---------------------------------------------------------------------------*/
 /* We define approximate_required_operations, approximate_calculate_metric, etc. */

static int approximate_required_operations(
    GraphAlgorithmContext* context_1,
    GraphAlgorithmContext* context_2,
    int* arr,
    int n,
    int smaller_n
)
{
    int required_operations = n - smaller_n;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i >= smaller_n || j >= smaller_n) {
                required_operations += context_1->graph_interface->get_edge(
                    context_1->graph_interface, arr[i] - 1, arr[j] - 1);
                continue;
            }
            int e1 = context_1->graph_interface->get_edge(context_1->graph_interface, arr[i] - 1, arr[j] - 1);
            int e2 = context_2->graph_interface->get_edge(context_2->graph_interface, i, j);
            required_operations += abs(e1 - e2);
        }
    }
    return required_operations;
}

static void generate_random_permutation(int* arr, int n) {
    for (int i = 0; i < n; i++) {
        int j = rand() % n;
        common_swap(&arr[i], &arr[j]);
    }
}
static int approximate_calculate_metric(
    GraphAlgorithmContext* context_1,
    GraphAlgorithmContext* context_2,
    int vertices_1,
    int vertices_2
)
{
    int* arr = (int*)malloc(vertices_1 * sizeof(int));
    for (int i = 0; i < vertices_1; i++) {
        arr[i] = i + 1;
    }
    srand((unsigned)time(NULL));
    generate_random_permutation(arr, vertices_1);

    int current_metric = approximate_required_operations(context_1, context_2, arr, vertices_1, vertices_2);
    int best_metric = current_metric;
    double temperature = INITIAL_TEMP;

    for (int iter = 0; iter < MAX_ITER; iter++) {
        int i = rand() % vertices_1;
        int j = rand() % vertices_1;
        common_swap(&arr[i], &arr[j]);
        int new_metric = approximate_required_operations(context_1, context_2, arr, vertices_1, vertices_2);
        if (new_metric < current_metric ||
            (common_exp((current_metric - new_metric) / temperature) > ((double)rand() / RAND_MAX))) {
            current_metric = new_metric;
            if (new_metric < best_metric) {
                best_metric = new_metric;
            }
        }
        else {
            common_swap(&arr[i], &arr[j]); /* revert */
        }
        temperature *= COOLING_RATE;
    }
    free(arr);
    return best_metric;
}

static int approximate_find_minimal_extension(void* graph, int vertices) {
    GraphAlgorithmContext* ctx = create_context(graph, vertices);
    int edge_additions = 0;
    for (int i = 0; i < vertices; i++) {
        for (int j = i + 1; j < vertices; j++) {
            if (ctx->graph_interface->get_edge(graph, i, j) == 0) {
                ctx->graph_interface->add_edge(graph, i, j, 1);
                edge_additions++;
                break;
            }
        }
    }
    destroy_context(ctx);
    return edge_additions;
}

static int approximate_count_maximal_cycles(void* graph, int vertices) {
    GraphAlgorithmContext* ctx = create_context(graph, vertices);
    int max_cycle_length = 0;
    for (int i = 0; i < vertices; i++) {
        int* visited = (int*)calloc(vertices, sizeof(int));
        int cycle_length = 0;
        int node = i;
        while (cycle_length < vertices) {
            visited[node] = 1;
            cycle_length++;
            int found_next = 0;
            for (int j = 0; j < vertices; j++) {
                if (!visited[j] && ctx->graph_interface->get_edge(graph, node, j) > 0) {
                    node = j;
                    found_next = 1;
                    break;
                }
            }
            if (!found_next) break;
        }
        if (cycle_length > max_cycle_length) {
            max_cycle_length = cycle_length;
        }
        free(visited);
    }
    destroy_context(ctx);
    return max_cycle_length;
}

static int approximate_find_maximal_cycles(void* graph, int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count)
{
    GraphAlgorithmContext* ctx = create_context(graph, vertices);
    *cycle_count = 0;
    *output_cycles = NULL;
    *cycle_sizes = NULL;
    for (int i = 0; i < vertices; i++) {
        int* cycle = (int*)malloc(vertices * sizeof(int));
        int length = 0;
        int* visited = (int*)calloc(vertices, sizeof(int));

        int node = i;
        while (length < vertices) {
            visited[node] = 1;
            cycle[length++] = node;
            int found_next = 0;
            for (int j = 0; j < vertices; j++) {
                if (!visited[j] && ctx->graph_interface->get_edge(graph, node, j) > 0) {
                    node = j;
                    found_next = 1;
                    break;
                }
            }
            if (!found_next) break;
        }
        (*cycle_count)++;
        *output_cycles = (int**)realloc(*output_cycles, (*cycle_count) * sizeof(int*));
        *cycle_sizes = (int*)realloc(*cycle_sizes, (*cycle_count) * sizeof(int));
        (*output_cycles)[*cycle_count - 1] = cycle;
        (*cycle_sizes)[*cycle_count - 1] = length;
        free(visited);
    }
    destroy_context(ctx);
    return *cycle_count;
}

static int approximate_find_cycles(void* graph, int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count)
{
    GraphAlgorithmContext* ctx = create_context(graph, vertices);
    *cycle_count = 0;
    *output_cycles = NULL;
    *cycle_sizes = NULL;
    for (int i = 0; i < vertices / 2; i++) {
        for (int j = 0; j < vertices / 2; j++) {
            if (ctx->graph_interface->get_edge(graph, i, j) > 0) {
                (*cycle_count)++;
                *output_cycles = (int**)realloc(*output_cycles, (*cycle_count) * sizeof(int*));
                *cycle_sizes = (int*)realloc(*cycle_sizes, (*cycle_count) * sizeof(int));
                int* cycle = (int*)malloc(3 * sizeof(int));
                cycle[0] = i;
                cycle[1] = j;
                cycle[2] = i;
                (*output_cycles)[*cycle_count - 1] = cycle;
                (*cycle_sizes)[*cycle_count - 1] = 3;
            }
        }
    }
    destroy_context(ctx);
    return *cycle_count;
}

static int approximate_count_hamiltonian_cycles(void* graph, int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count)
{
    GraphAlgorithmContext* ctx = create_context(graph, vertices);
    *cycle_count = 0;
    *output_cycles = NULL;
    *cycle_sizes = NULL;
    for (int start = 0; start < vertices; start++) {
        int* path = (int*)malloc(vertices * sizeof(int));
        path[0] = start;
        int current = start;
        int step = 1;
        for (; step < vertices; step++) {
            for (int nxt = 0; nxt < vertices; nxt++) {
                if (ctx->graph_interface->get_edge(graph, current, nxt) > 0) {
                    path[step] = nxt;
                    current = nxt;
                    break;
                }
            }
        }
        if (ctx->graph_interface->get_edge(graph, current, start) > 0) {
            (*cycle_count)++;
            *output_cycles = (int**)realloc(*output_cycles, (*cycle_count) * sizeof(int*));
            *cycle_sizes = (int*)realloc(*cycle_sizes, (*cycle_count) * sizeof(int));
            (*output_cycles)[*cycle_count - 1] = path;
            (*cycle_sizes)[*cycle_count - 1] = vertices;
        }
        else {
            free(path);
        }
    }
    destroy_context(ctx);
    return *cycle_count;
}

/*-----------------------------------------------------------------------------
 * Wrapper to get graph size from the interface
 *---------------------------------------------------------------------------*/
static int calculate_size_wrapper(void* graph) {
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

/*-----------------------------------------------------------------------------
 * Checking if there's at least 1 Hamiltonian cycle (hasHamiltonianCycle)
 *---------------------------------------------------------------------------*/
static void checkCycle(GraphAlgorithmContext* context, int start, int current, int depth,
    int* path, bool* visited, bool* foundOne);

static bool hasHamiltonianCycle(GraphAlgorithmContext* context)
{
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
static void checkCycle(GraphAlgorithmContext* context, int start, int current, int depth,
    int* path, bool* visited, bool* foundOne)
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

/*-----------------------------------------------------------------------------
 * explore_extensions: for minimal extension
 *---------------------------------------------------------------------------*/
static void explore_extensions(GraphAlgorithmContext* context, int vertices, int addedEdges, int* minEdgesNeeded);
static void explore_extensions(GraphAlgorithmContext* context, int vertices, int addedEdges, int* minEdgesNeeded)
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
                    gi->add_edge(gi, i, j, -1); /* remove edge */
                }
            }
        }
    }
}

/*-----------------------------------------------------------------------------
 * GraphAlgorithm structure
 *---------------------------------------------------------------------------*/
typedef struct GraphAlgorithmTag {
    int  (*calculate_size)(void* graph);
    int  (*find_cycles)(void* graph, int vertices, int*** output_cycles, int** cycle_sizes, int* cycle_count);
    int  (*count_hamiltonian_cycles)(void* graph, int vertices, int*** output_cycles, int** cycle_sizes, int* cycle_count);
    void (*calculate_metric)(void* graph1, int v1, void* graph2, int v2, int* exact_metric, int* approximate_metric);
    int  (*find_minimal_extension)(void* graph, int vertices);
    int  (*count_maximal_cycles)(void* graph, int vertices);
    int  (*find_maximal_cycles)(void* graph, int vertices, int*** output_cycles, int** cycle_sizes, int* cycle_count);
} GraphAlgorithm;

/*
 * Local wrapper prototypes
 */
static int  find_cycles_wrapper(void* graph, int vertices, int*** outA, int** outS, int* outCount);
static int  count_hamiltonian_cycles_wrapper(void* graph, int vertices, int*** outA, int** outS, int* outCount);
static void calculate_metric_wrapper(void* g1, int v1, void* g2, int v2, int* exact_metric, int* approximate_metric);
static int  find_minimal_extension_wrapper(void* graph, int v);
static int  count_maximal_cycles_wrapper(void* graph, int v);
static int  find_maximal_cycles_wrapper(void* graph, int v, int*** outA, int** outS, int* outC);

/* Our "default_algorithm" in object */
static GraphAlgorithm default_algorithm = {
    .calculate_size = calculate_size_wrapper,
    .find_cycles = find_cycles_wrapper,
    .count_hamiltonian_cycles = count_hamiltonian_cycles_wrapper,
    .calculate_metric = calculate_metric_wrapper,
    .find_minimal_extension = find_minimal_extension_wrapper,
    .count_maximal_cycles = count_maximal_cycles_wrapper,
    .find_maximal_cycles = find_maximal_cycles_wrapper
};

/* EXACT: find_cycles */
static int find_cycles_wrapper(
    void* graph,
    int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count)
{
    if (vertices >= THRESHOLD) {
        return approximate_find_cycles(graph, vertices, output_cycles, cycle_sizes, cycle_count);
    }
    GraphAlgorithmContext* ctx = create_context(graph, vertices);
    if (!ctx) return 0;
    CycleList   cycleList;
    initCycleList(&cycleList);

    StringList  uniqueCycles;
    initStringList(&uniqueCycles);

    int localCount = 0;
    for (int start = 0; start < vertices; start++) {
        int* stack = (int*)malloc(vertices * sizeof(int));
        bool* visited = (bool*)calloc(vertices, sizeof(bool));
        if (!stack || !visited) {
            free(stack);
            free(visited);
            continue;
        }
        dfs_findCycles(ctx, start, start, stack, 0,
            visited, &uniqueCycles, &cycleList, &localCount);
        free(stack);
        free(visited);
    }
    *output_cycles = cycleList.cycles;
    *cycle_sizes = cycleList.sizes;
    *cycle_count = cycleList.count;

    freeStringList(&uniqueCycles);
    destroy_context(ctx);
    return cycleList.count;
}

/* EXACT: count_hamiltonian_cycles */
static int count_hamiltonian_cycles_wrapper(
    void* graph,
    int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count)
{
    if (vertices >= THRESHOLD) {
        return approximate_count_hamiltonian_cycles(graph, vertices, output_cycles, cycle_sizes, cycle_count);
    }
    GraphAlgorithmContext* ctx = create_context(graph, vertices);
    if (!ctx) return 0;

    CycleList cycleList;
    initCycleList(&cycleList);

    StringList uniqueCycles;
    initStringList(&uniqueCycles);

    int localCount = 0;
    /* Try each vertex as a start node */
    for (int start = 0; start < vertices; start++) {
        int* path = (int*)malloc(vertices * sizeof(int));
        bool* visited = (bool*)calloc(vertices, sizeof(bool));
        if (!path || !visited) {
            free(path);
            free(visited);
            continue;
        }
        backtrack_hamiltonian(ctx, start, start, 1, path, visited,
            &uniqueCycles, &cycleList, &localCount);
        free(path);
        free(visited);
    }
    *output_cycles = cycleList.cycles;
    *cycle_sizes = cycleList.sizes;
    *cycle_count = cycleList.count;

    freeStringList(&uniqueCycles);
    destroy_context(ctx);
    return cycleList.count;
}

static int calculate_required_operations(
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

static int calculate_graph_metric(
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
    if (!arr) {
        fprintf(stderr, "Memory allocation error in calculate_graph_metric.\n");
        return -1;
    }
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

/* EXACT: calculate_metric */
static void calculate_metric_wrapper(
    void* g1, int v1,
    void* g2, int v2,
    int* exact_metric,
    int* approximate_metric)
{
    if (common_max(v1, v2) == v2) {
        /* swap so g1 always has >= # vertices */
        void* tmp = g1;  g1 = g2;   g2 = tmp;
        int tv = v1;     v1 = v2;   v2 = tv;
    }
    GraphAlgorithmContext* ctx1 = create_context(g1, v1);
    GraphAlgorithmContext* ctx2 = create_context(g2, v2);
    if (!ctx1 || !ctx2) {
        if (ctx1) destroy_context(ctx1);
        if (ctx2) destroy_context(ctx2);
        return;
    }
    clock_t start = clock();
    *exact_metric = calculate_graph_metric(ctx1, ctx2, v1, v2);
    clock_t end = clock();
    printf("Time taken for exact metric: %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

    start = clock();
    *approximate_metric = approximate_calculate_metric(ctx1, ctx2, v1, v2);
    end = clock();
    printf("Time taken for approximate metric: %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

    destroy_context(ctx1);
    destroy_context(ctx2);
}

/* EXACT: find_minimal_extension */
static int find_minimal_extension_wrapper(void* graph, int vertices) {
    if (vertices >= THRESHOLD) {
        return approximate_find_minimal_extension(graph, vertices);
    }
    GraphAlgorithmContext* ctx = create_context(graph, vertices);
    if (!ctx) return 0;

    int minEdgesNeeded = INT_MAX;

    /* Check if there's already a Hamiltonian cycle */
    bool foundOne = false;
    for (int i = 0; i < ctx->vertices && !foundOne; i++) {
        int* path = (int*)malloc(ctx->vertices * sizeof(int));
        bool* visited = (bool*)calloc(ctx->vertices, sizeof(bool));
        if (!path || !visited) {
            free(path);
            free(visited);
            continue;
        }
        checkCycle(ctx, i, i, 1, path, visited, &foundOne);
        free(path);
        free(visited);
    }
    if (foundOne) {
        destroy_context(ctx);
        return 0;
    }
    explore_extensions(ctx, vertices, 0, &minEdgesNeeded);
    destroy_context(ctx);
    if (minEdgesNeeded == INT_MAX) return 0;
    return minEdgesNeeded;
}

/* EXACT: count_maximal_cycles => length of longest cycle */
static int count_maximal_cycles_wrapper(void* graph, int vertices) {
    if (vertices >= THRESHOLD) {
        return approximate_count_maximal_cycles(graph, vertices);
    }
    GraphAlgorithmContext* ctx = create_context(graph, vertices);
    if (!ctx) return 0;

    int maxCycleLength = 0;
    for (int start = 0; start < vertices; start++) {
        bool* visited = (bool*)calloc(vertices, sizeof(bool));
        if (!visited) continue;
        dfs_maxCycleLength(ctx, start, start, 1, visited, &maxCycleLength);
        free(visited);
    }
    destroy_context(ctx);
    return maxCycleLength;
}

/* EXACT: find_maximal_cycles => enumerates & returns all cycles of that length */
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
            output_cycles, cycle_sizes, cycle_count);
    }
    GraphAlgorithmContext* ctx = create_context(graph, vertices);
    if (!ctx) {
        *output_cycles = NULL;
        *cycle_sizes = NULL;
        *cycle_count = 0;
        return 0;
    }

    /* 1) find maximum cycle length */
    int maxCycleLength = 0;
    for (int start = 0; start < vertices; start++) {
        bool* visited = (bool*)calloc(vertices, sizeof(bool));
        if (!visited) continue;
        dfs_maxCycleLength(ctx, start, start, 1, visited, &maxCycleLength);
        free(visited);
    }

    /* 2) collect all cycles matching max length */
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
        dfs_findMaxCycles(ctx, start, start, stack, 0, visited,
            &uniqueCycles, &cycleList, &maxCycleLength, &foundCount);
        free(stack);
        free(visited);
    }
    *output_cycles = cycleList.cycles;
    *cycle_sizes = cycleList.sizes;
    *cycle_count = cycleList.count;

    destroy_context(ctx);
    return cycleList.count;
}

/*-----------------------------------------------------------------------------
 * "print cycles" in a "0 -> 1 -> 2" format
 *---------------------------------------------------------------------------*/
void print_cycles(GraphInterface* multigraph, int** output_cycles, int* cycle_sizes, int cycle_count) {
    for (int i = 0; i < cycle_count; i++) {
        for (int j = 0; j < cycle_sizes[i]; j++) {
            printf("%d", output_cycles[i][j]);
            if (j < cycle_sizes[i] - 1) {
                printf(" -> ");
            }
        }
        printf("\n");
        free(output_cycles[i]);  /* free each cycle array */
    }
    free(output_cycles); /* free the array of pointers */
    free(cycle_sizes);   /* free the sizes array */
}


void init_graph_array(GraphArray* arr, size_t capacity) {
    arr->data = (GraphInterface**)malloc(capacity * sizeof(GraphInterface*));
    arr->size = 0;
    arr->capacity = capacity;
}
void add_graph(GraphArray* arr, GraphInterface* graph) {
    if (arr->size >= arr->capacity) {
        arr->capacity *= 2;
        arr->data = (GraphInterface**)realloc(arr->data, arr->capacity * sizeof(GraphInterface*));
    }
    arr->data[arr->size++] = graph;
}
void free_graph_array(GraphArray* arr) {
    free(arr->data);
}

/* argument handling + open_file_with_retry */
void handle_arguments(int argc, char* argv[], Config* config) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    config->input_file = argv[1];
    if (argc == 3) {
        config->metric_optional_file = argv[2];
    }
}
FILE* open_file_with_retry(const char* filename) {
    FILE* file;
    do {
        file = fopen(filename, "r");
    } while (!file && errno == EINTR);
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    return file;
}

/*
 * analyze_multigraph
 *  => prints
 *     1) All cycles
 *     2) Hamiltonian cycles
 *     3) Minimal extension
 *     4) Maximal cycle length
 *     5) Maximal cycles
 */
extern GraphAlgorithm default_algorithm;  /* Our global struct above. */

void analyze_multigraph(GraphInterface* multigraph) {
    printf("Analyzing Multigraph:\n");
    printf("------------------------------------------------\n");

    /* Print a size measure (number of edges, etc.) */
    int graph_size = default_algorithm.calculate_size(multigraph);
    printf("Graph size (number of edges): %d\n", graph_size);

    /* 1) All cycles */
    {
        int** cyc_out = NULL;
        int* cyc_sizes = NULL;
        int cyc_count_num = 0;
        int cyc_count = default_algorithm.find_cycles(
            multigraph,
            multigraph->vertices,
            &cyc_out,
            &cyc_sizes,
            &cyc_count_num
        );
        printf("All cycles: %d\n", cyc_count);
        print_cycles(multigraph, cyc_out, cyc_sizes, cyc_count);
    }

    /* 2) Hamiltonian cycles */
    {
        int** ham_out = NULL;
        int* ham_sizes = NULL;
        int ham_count_num = 0;
        int ham_count = default_algorithm.count_hamiltonian_cycles(
            multigraph,
            multigraph->vertices,
            &ham_out,
            &ham_sizes,
            &ham_count_num
        );
        printf("Hamiltonian cycles: %d\n", ham_count);
        print_cycles(multigraph, ham_out, ham_sizes, ham_count);
    }

    /* 3) minimal extension for Hamiltonian cycle */
    int min_ext = default_algorithm.find_minimal_extension(multigraph, multigraph->vertices);
    printf("Minimal extension for Hamiltonian cycle: %d\n", min_ext);

    /* 4) maximal cycle length */
    int max_len = default_algorithm.count_maximal_cycles(multigraph, multigraph->vertices);
    printf("Maximal cycle length: %d\n", max_len);

    /* 5) find all cycles of that length */
    {
        int** max_out = NULL;
        int* max_sizes = NULL;
        int max_count_var = 0;
        int max_count = default_algorithm.find_maximal_cycles(
            multigraph,
            multigraph->vertices,
            &max_out,
            &max_sizes,
            &max_count_var
        );
        printf("Maximal cycles: %d\n", max_count);
        print_cycles(multigraph, max_out, max_sizes, max_count);
    }

    printf("------------------------------------------------\n\n");
}

/* Read multiple graphs from a file, analyze each */
void process_multigraphs(const char* file_name, GraphArray* multigraphs_to_compare) {
    FILE* file = open_file_with_retry(file_name);
    static int file_counter = 0;
    printf("Processing file %d: %s\n", ++file_counter, file_name);

    int num_graphs;
    fscanf(file, "%d", &num_graphs);
    printf("Processing %d graphs.\n\n", num_graphs);

    for (int i = 0; i < num_graphs; i++) {
        printf("Graph %d:\n", i + 1);
        int vertices;
        fscanf(file, "%d", &vertices);

        /* create a multigraph from "graph.c" */
        GraphInterface* multigraph = create_multigraph(vertices);
        if (!multigraph) {
            fprintf(stderr, "Failed to initialize graph interface for graph %d.\n", i + 1);
            continue;
        }

        if (i == 0) {
            add_graph(multigraphs_to_compare, multigraph);
        }

        /* read adjacency matrix */
        for (int j = 0; j < vertices; j++) {
            for (int k = 0; k < vertices; k++) {
                int weight;
                fscanf(file, "%d", &weight);
                if (weight > 0) {
                    multigraph->add_edge(multigraph, j, k, weight);
                }
            }
        }
        analyze_multigraph(multigraph);
    }
    fclose(file);
}

void process_metrics(GraphInterface* multigraph_1, GraphInterface* multigraph_2) {
    printf("Comparing graphs:\n");
    printf("------------------------------------------------\n");

    printf("First graph with '%d' vertices and '%d' edges\n",
        multigraph_1->vertices,
        multigraph_1->calculate_size(multigraph_1));
    printf("Second graph with '%d' vertices and '%d' edges\n",
        multigraph_2->vertices,
        multigraph_2->calculate_size(multigraph_2));

    int exact_metric, approximate_metric;
    default_algorithm.calculate_metric(
        multigraph_1, multigraph_1->vertices,
        multigraph_2, multigraph_2->vertices,
        &exact_metric,
        &approximate_metric
    );
    printf("Graph similarity metric between graphs: %d\n", exact_metric);
    printf("Approximate graph similarity metric between graphs: %d\n", approximate_metric);
    printf("------------------------------------------------\n\n");
}

/* External config from "config.h" */
extern Config* get_config();

int main(int argc, char* argv[]) {
    Config* config = get_config();
    handle_arguments(argc, argv, config);

    GraphArray multigraphs_to_compare;
    init_graph_array(&multigraphs_to_compare, 2);

    process_multigraphs(config->input_file, &multigraphs_to_compare);

    if (config->metric_optional_file) {
        process_multigraphs(config->metric_optional_file, &multigraphs_to_compare);
        if (multigraphs_to_compare.size >= 2) {
            process_metrics(multigraphs_to_compare.data[0], multigraphs_to_compare.data[1]);
        }
    }

    free_graph_array(&multigraphs_to_compare);
    return EXIT_SUCCESS;
}
