#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>
#include <windows.h>  

#include "graph.h"
#include "utils.h"
#include "graph_interface.h"
#include "../include/config.h"
#include "common_utils.h"
#include "stack.h"   
#include "matching.h"

#include "graph_algorithm_context.h"
#include "cycle_list.h"
#include "string_list.h"
#include "graph_algorithm.h"
/*-----------------------------------------------------------------------------
 * Macros & Constants
 *---------------------------------------------------------------------------*/
#define THRESHOLD     10      /* If vertices >= THRESHOLD, use approximate */
#define MAX_ITER      20000   /* For approximate sim-anneal logic */
#define INITIAL_TEMP  200.0
#define COOLING_RATE  0.99

 /*-----------------------------------------------------------------------------
  * GraphAlgorithmContext
  *---------------------------------------------------------------------------*/
//typedef struct {
//    GraphInterface* graph_interface;
//    int vertices;
//} GraphAlgorithmContext;
//
//static GraphAlgorithmContext* create_context(void* graph, int vertices) {
//    GraphAlgorithmContext* ctx = (GraphAlgorithmContext*)malloc(sizeof(GraphAlgorithmContext));
//    if (!ctx) {
//        fprintf(stderr, "Error: failed to allocate GraphAlgorithmContext\n");
//        return NULL;
//    }
//    ctx->graph_interface = (GraphInterface*)graph;
//    ctx->vertices = vertices;
//    return ctx;
//}
//static void destroy_context(GraphAlgorithmContext* context) {
//    if (context) free(context);
//}

/*-----------------------------------------------------------------------------
 * CycleList: store arrays-of-int plus their lengths
 *---------------------------------------------------------------------------*/
//typedef struct {
//    int** cycles;
//    int* sizes;
//    int   count;
//    int   capacity;
//} CycleList;
//
//#define INITIAL_CYCLELIST_CAPACITY 16
//
//static void initCycleList(CycleList* cl) {
//    cl->count = 0;
//    cl->capacity = INITIAL_CYCLELIST_CAPACITY;
//    cl->cycles = (int**)malloc(cl->capacity * sizeof(int*));
//    cl->sizes = (int*)malloc(cl->capacity * sizeof(int));
//}
//static void freeCycleList(CycleList* cl) {
//    if (!cl) return;
//    for (int i = 0; i < cl->count; i++) {
//        free(cl->cycles[i]);
//    }
//    free(cl->cycles);
//    free(cl->sizes);
//    cl->count = 0;
//    cl->capacity = 0;
//}
//static void addCycle(CycleList* cl, const int* cycle, int length) {
//    if (cl->count == cl->capacity) {
//        cl->capacity *= 2;
//        cl->cycles = (int**)realloc(cl->cycles, cl->capacity * sizeof(int*));
//        cl->sizes = (int*)realloc(cl->sizes, cl->capacity * sizeof(int));
//        if (!cl->cycles || !cl->sizes) {
//            fprintf(stderr, "Memory reallocation failed in addCycle.\n");
//            return;
//        }
//    }
//    int* newCycle = (int*)malloc(length * sizeof(int));
//    memcpy(newCycle, cycle, length * sizeof(int));
//
//    cl->cycles[cl->count] = newCycle;
//    cl->sizes[cl->count] = length;
//    cl->count++;
//}

/*-----------------------------------------------------------------------------
 * StringList: to store unique string representations (avoid duplicates)
 *---------------------------------------------------------------------------*/
//typedef struct {
//    char** data;
//    int    count;
//    int    capacity;
//} StringList;
//
//#define INITIAL_STRINGLIST_CAPACITY 16
//
//static void initStringList(StringList* sl) {
//    sl->count = 0;
//    sl->capacity = INITIAL_STRINGLIST_CAPACITY;
//    sl->data = (char**)malloc(sl->capacity * sizeof(char*));
//}
//static void freeStringList(StringList* sl) {
//    if (!sl) return;
//    for (int i = 0; i < sl->count; i++) {
//        free(sl->data[i]);
//    }
//    free(sl->data);
//    sl->count = 0;
//    sl->capacity = 0;
//}
///* Check if we already have a cycle in string form (to avoid duplicates). */
//static bool stringListContains(const StringList* sl, const char* str) {
//    for (int i = 0; i < sl->count; i++) {
//        if (strcmp(sl->data[i], str) == 0) return true;
//    }
//    return false;
//}
//static void addStringToList(StringList* sl, const char* str) {
//    if (sl->count == sl->capacity) {
//        sl->capacity *= 2;
//        sl->data = (char**)realloc(sl->data, sl->capacity * sizeof(char*));
//    }
//    if (!sl->data) {
//        fprintf(stderr, "Memory reallocation failed in addStringToList.\n");
//        return;
//    }
//    sl->data[sl->count] = (char*)malloc(strlen(str) + 1);
//    strcpy(sl->data[sl->count], str);
//    sl->count++;
//}

/*-----------------------------------------------------------------------------
 * Helpers: normalizeCycle & nextPermutation (form including the call cycles
    including the permutations in the cycles)
 *---------------------------------------------------------------------------*/
//char* normalizeCycle(int* cycle, int length) {
//    if (length <= 0) return NULL;
//
//    // Find the smallest vertex in the cycle
//    int minIndex = 0;
//    for (int i = 1; i < length; i++) {
//        if (cycle[i] < cycle[minIndex]) {
//            minIndex = i;
//        }
//    }
//
//    // Create the normalized cycle starting from the smallest vertex
//    char* result = (char*)malloc(length * 12 + 1);
//    result[0] = '\0';
//    for (int i = 0; i < length; i++) {
//        char buf[32];
//        sprintf(buf, "%d-", cycle[(minIndex + i) % length]);
//        strcat(result, buf);
//    }
//    return result;
//}

//char* normalizeCycle(int* cycle, int length) {
//    if (length <= 0) return NULL;
//
//    int minIndex = 0;
//    for (int i = 1; i < length; i++) {
//        if (cycle[i] < cycle[minIndex]) {
//            minIndex = i;
//        }
//    }
//
//    char forward[256] = { 0 };
//    char reverse[256] = { 0 };
//
//    for (int i = 0; i < length; i++) {
//        char buffer[12];
//        sprintf(buffer, "%d-", cycle[(minIndex + i) % length]);
//        strcat(forward, buffer);
//    }
//
//    for (int i = 0; i < length; i++) {
//        char buffer[12];
//        sprintf(buffer, "%d-", cycle[(minIndex - i + length) % length]);
//        strcat(reverse, buffer);
//    }
//
//    char* result = (strcmp(forward, reverse) < 0) ? forward : reverse;
//
//    char* canonical = (char*)malloc(strlen(result) + 1);
//    strcpy(canonical, result);
//    return canonical;
//}
//
//
///* nextPermutation for exact metric calculation */
//static bool nextPermutation(int* arr, int n) {
//    int i = n - 2;
//    while (i >= 0 && arr[i] >= arr[i + 1]) i--;
//    if (i < 0) return false;
//    int j = n - 1;
//    while (arr[j] <= arr[i]) j--;
//    common_swap(&arr[i], &arr[j]);
//    int left = i + 1, right = n - 1;
//    while (left < right) {
//        common_swap(&arr[left], &arr[right]);
//        left++;
//        right--;
//    }
//    return true;
//}
//
///*-----------------------------------------------------------------------------
// * EXACT DFS routines
// *---------------------------------------------------------------------------*/
//
// /*
//  * (1) Non-Hamiltonian cycles: all simple cycles (no repeated vertices),
//  *     of length >= 2 edges.
//  */
//static void dfs_findCycles(
//    GraphAlgorithmContext* ctx,
//    int start,
//    int current,
//    Stack* stack,
//    bool* visited,
//    StringList* uniqueCycles,
//    CycleList* cycleList,
//    int* localCount
//) {
//    push(stack, current);
//    visited[current] = true; 
//
//    GraphInterface* gi = ctx->graph_interface;
//    int stackLen = stack->top + 1;
//
//    int out_degree;
//    int* neighbors = gi->get_all_edges_from_vertex(gi, current, &out_degree);
//    /*printf("Node %d has %d neighbors: ", current, out_degree);
//    for (int i = 0; i < out_degree; i++) {
//        printf("%d ", neighbors[i]);
//    }
//    printf("\n");*/
//
//    for (int i = 0; i < out_degree; i++) {
//        int nxt = neighbors[i];
//
//        if (nxt == start && stackLen >= 2) {
//            // **Valid cycle found**
//            int cycleLength = stackLen + 1;
//            int* cycleArray = (int*)malloc(cycleLength * sizeof(int));
//            for (int j = 0; j < stackLen; j++) {
//                cycleArray[j] = stack->data[j];
//            }
//            cycleArray[cycleLength - 1] = start; // **Close the cycle**
//
//            /* printf("[Vefore CYCLE FOUND]: ");
//            for (int j = 0; j < cycleLength; j++) {
//                printf("%d ", cycleArray[j]);
//            }
//            printf("\n");*/
//            char* canonical = normalizeCycle(cycleArray, cycleLength);
//            if (!stringListContains(uniqueCycles, canonical)) {
//               /* printf("[CYCLE FOUND]: ");
//                for (int j = 0; j < cycleLength; j++) {
//                    printf("%d ", cycleArray[j]);
//                }
//                printf("\n");*/
//
//                addStringToList(uniqueCycles, canonical);
//                addCycle(cycleList, cycleArray, cycleLength);
//                (*localCount)++;
//            }
//
//            free(canonical);
//            free(cycleArray);
//        }
//        else if (!visited[nxt]) {
//            dfs_findCycles(ctx, start, nxt, stack, visited, uniqueCycles, cycleList, localCount);
//        }
//    }
//
//    pop(stack);
//    visited[current] = false;
//    free(neighbors);
//}
//
//
//
///*
// * (2) Hamiltonian cycles (backtrack).
// *     We'll also keep the path in a stack for clarity.
// */
//static void backtrack_hamiltonian(
//    GraphAlgorithmContext* context,
//    int start,
//    int current,
//    Stack* stack,
//    bool* visited,
//    StringList* uniqueCycles,
//    CycleList* cycleList,
//    int* localCount
//)
//{
//    push(stack, current);
//    visited[current] = true;
//
//    GraphInterface* gi = context->graph_interface;
//    int depth = stack->top + 1;  // how many distinct vertices visited so far
//
//    if (depth == context->vertices) {
//        // Try to close the cycle back to 'start'
//        int w = gi->get_edge(gi, current, start);
//        if (w > 0) {
//            int cycleLength = depth + 1; 
//            int* cycleArray = (int*)malloc(cycleLength * sizeof(int));
//            for (int i = 0; i < depth; i++) {
//                cycleArray[i] = stack->data[i];
//            }
//            cycleArray[depth] = start; // close cycle
//
//            char* canonical = normalizeCycle(cycleArray, cycleLength);
//            if (!stringListContains(uniqueCycles, canonical)) {
//                addStringToList(uniqueCycles, canonical);
//                addCycle(cycleList, cycleArray, cycleLength);
//                (*localCount)++;
//            }
//            free(canonical);
//            free(cycleArray);
//        }
//    }
//    else {
//        // Try all possible next vertices that aren't visited
//        for (int nxt = 0; nxt < context->vertices; nxt++) {
//            int w2 = gi->get_edge(gi, current, nxt);
//            if (w2 > 0 && !visited[nxt]) {
//                backtrack_hamiltonian(context, start, nxt, stack,
//                    visited, uniqueCycles, cycleList, localCount);
//            }
//        }
//    }
//
//    // Backtrack
//    pop(stack);
//    visited[current] = false;
//}
//
///*
// * (3) Maximal cycle length: we only need a "depth" count. The maximum
// *     simple cycle length found across any DFS is stored in *maxLen.
// */
//static void dfs_maxCycleLength(
//    GraphAlgorithmContext* context,
//    int start,
//    int current,
//    Stack* stack,
//    bool* visited,
//    int* maxLen
//)
//{
//    push(stack, current);
//    visited[current] = true;
//    int depth = stack->top + 1;
//
//    GraphInterface* gi = context->graph_interface;
//    for (int nxt = 0; nxt < context->vertices; nxt++) {
//        int w = gi->get_edge(gi, current, nxt);
//        if (w > 0) {
//            // If we returned to 'start', we have a cycle. The length is depth+1.
//            if (nxt == start && depth >= 1) {
//                if (depth > *maxLen) {
//                    *maxLen = depth;
//                }
//            }
//            else if (!visited[nxt]) {
//                dfs_maxCycleLength(context, start, nxt, stack, visited, maxLen);
//            }
//        }
//    }
//    // backtrack
//    pop(stack);
//    visited[current] = false;
//}
//
///*
// * (4) find all cycles exactly matching the maximum length *maxCycleLen
// */
//static void dfs_findMaxCycles(
//    GraphAlgorithmContext* context,
//    int start,
//    int current,
//    Stack* stack,
//    bool* visited,
//    StringList* uniqueCycles,
//    CycleList* outputList,
//    int* maxCycleLen,
//    int* foundCount
//)
//{
//    push(stack, current);
//    visited[current] = true;
//    int stackLen = stack->top + 1;
//
//    GraphInterface* gi = context->graph_interface;
//    for (int nxt = 0; nxt < context->vertices; nxt++) {
//        int w = gi->get_edge(gi, current, nxt);
//        if (w > 0) {
//            // If we closed a cycle of length == *maxCycleLen, record it
//            if (nxt == start && stackLen == *maxCycleLen) {
//                int cycleLength = stackLen + 1;  // e.g. 3 -> 4 array
//                int* cycleArray = (int*)malloc(cycleLength * sizeof(int));
//                for (int i = 0; i < stackLen; i++) {
//                    cycleArray[i] = stack->data[i];
//                }
//                cycleArray[stackLen] = start;
//
//                char* canonical = normalizeCycle(cycleArray, cycleLength);
//                if (!stringListContains(uniqueCycles, canonical)) {
//                    addStringToList(uniqueCycles, canonical);
//                    addCycle(outputList, cycleArray, cycleLength);
//                    (*foundCount)++;
//                }
//                free(canonical);
//                free(cycleArray);
//            }
//            else if (!visited[nxt] && stackLen < *maxCycleLen) {
//                dfs_findMaxCycles(context, start, nxt, stack, visited,
//                    uniqueCycles, outputList, maxCycleLen, foundCount);
//            }
//        }
//    }
//    pop(stack);
//    visited[current] = false;
//}

/*-----------------------------------------------------------------------------
 * Approximate logic stubs (for large graphs >= THRESHOLD)
 *---------------------------------------------------------------------------*/
//static int approximate_required_operations(
//    GraphAlgorithmContext* context_1,
//    GraphAlgorithmContext* context_2,
//    int* arr,
//    int n,
//    int smaller_n
//)
//{
//    int required_operations = n - smaller_n;
//    for (int i = 0; i < n; i++) {
//        for (int j = 0; j < n; j++) {
//            if (i >= smaller_n || j >= smaller_n) {
//                required_operations += context_1->graph_interface->get_edge(
//                    context_1->graph_interface, arr[i] - 1, arr[j] - 1);
//                continue;
//            }
//            int e1 = context_1->graph_interface->get_edge(context_1->graph_interface, arr[i] - 1, arr[j] - 1);
//            int e2 = context_2->graph_interface->get_edge(context_2->graph_interface, i, j);
//            required_operations += abs(e1 - e2);
//        }
//    }
//    return required_operations;
//}
//static void generate_random_permutation(int* arr, int n) {
//    for (int i = 0; i < n; i++) {
//        int j = rand() % n;
//        common_swap(&arr[i], &arr[j]);
//    }
//}
//static int approximate_calculate_metric(
//    GraphAlgorithmContext* context_1,
//    GraphAlgorithmContext* context_2,
//    int vertices_1,
//    int vertices_2
//)
//{
//    int* arr = (int*)malloc(vertices_1 * sizeof(int));
//    for (int i = 0; i < vertices_1; i++) {
//        arr[i] = i + 1;
//    }
//    srand((unsigned)time(NULL));
//    generate_random_permutation(arr, vertices_1);
//
//    int current_metric = approximate_required_operations(context_1, context_2, arr, vertices_1, vertices_2);
//    int best_metric = current_metric;
//    double temperature = INITIAL_TEMP;
//
//    for (int iter = 0; iter < MAX_ITER; iter++) {
//        int i = rand() % vertices_1;
//        int j = rand() % vertices_1;
//        common_swap(&arr[i], &arr[j]);
//        int new_metric = approximate_required_operations(context_1, context_2, arr, vertices_1, vertices_2);
//
//        if (new_metric < current_metric ||
//            (common_exp((current_metric - new_metric) / temperature) > ((double)rand() / RAND_MAX))) {
//            current_metric = new_metric;
//            if (new_metric < best_metric) {
//                best_metric = new_metric;
//            }
//        }
//        else {
//            common_swap(&arr[i], &arr[j]); /* revert */
//        }
//        temperature *= COOLING_RATE;
//    }
//    free(arr);
//    return best_metric;
//}
//static int approximate_find_minimal_extension(void* graph, int vertices) {
//    GraphAlgorithmContext* ctx = create_context(graph, vertices);
//    int edge_additions = 0;
//    for (int i = 0; i < vertices; i++) {
//        for (int j = i + 1; j < vertices; j++) {
//            if (ctx->graph_interface->get_edge(graph, i, j) == 0) {
//                ctx->graph_interface->add_edge(graph, i, j, 1);
//                edge_additions++;
//                break;
//            }
//        }
//    }
//    destroy_context(ctx);
//    return edge_additions;
//}
//static int approximate_count_maximal_cycles(void* graph, int vertices) {
//    GraphAlgorithmContext* ctx = create_context(graph, vertices);
//    int max_cycle_length = 0;
//    for (int i = 0; i < vertices; i++) {
//        int* visited = (int*)calloc(vertices, sizeof(int));
//        int cycle_length = 0;
//        int node = i;
//        while (cycle_length < vertices) {
//            visited[node] = 1;
//            cycle_length++;
//            int found_next = 0;
//            for (int j = 0; j < vertices; j++) {
//                if (!visited[j] && ctx->graph_interface->get_edge(graph, node, j) > 0) {
//                    node = j;
//                    found_next = 1;
//                    break;
//                }
//            }
//            if (!found_next) break;
//        }
//        if (cycle_length > max_cycle_length) {
//            max_cycle_length = cycle_length;
//        }
//        free(visited);
//    }
//    destroy_context(ctx);
//    return max_cycle_length;
//}
//static int approximate_find_maximal_cycles(void* graph, int vertices,
//    int*** output_cycles,
//    int** cycle_sizes,
//    int* cycle_count)
//{
//    GraphAlgorithmContext* ctx = create_context(graph, vertices);
//    *cycle_count = 0;
//    *output_cycles = NULL;
//    *cycle_sizes = NULL;
//    for (int i = 0; i < vertices; i++) {
//        int* cycle = (int*)malloc(vertices * sizeof(int));
//        int length = 0;
//        int* visited = (int*)calloc(vertices, sizeof(int));
//
//        int node = i;
//        while (length < vertices) {
//            visited[node] = 1;
//            cycle[length++] = node;
//            int found_next = 0;
//            for (int j = 0; j < vertices; j++) {
//                if (!visited[j] && ctx->graph_interface->get_edge(graph, node, j) > 0) {
//                    node = j;
//                    found_next = 1;
//                    break;
//                }
//            }
//            if (!found_next) break;
//        }
//        (*cycle_count)++;
//        *output_cycles = (int**)realloc(*output_cycles, (*cycle_count) * sizeof(int*));
//        *cycle_sizes = (int*)realloc(*cycle_sizes, (*cycle_count) * sizeof(int));
//        (*output_cycles)[*cycle_count - 1] = cycle;
//        (*cycle_sizes)[*cycle_count - 1] = length;
//        free(visited);
//    }
//    destroy_context(ctx);
//    return *cycle_count;
//}
///*
//    Approximation algorithms for cycle and path partitions in complete graphs https://arxiv.org/pdf/2311.11332
//*/
//static int approximate_find_cycles(void* graph, int vertices,
//    int*** output_cycles,
//    int** cycle_sizes,
//    int* cycle_count)
//{
//    GraphAlgorithmContext* ctx = create_context(graph, vertices);
//    if (!ctx) {
//        *output_cycles = NULL;
//        *cycle_sizes = NULL;
//        *cycle_count = 0;
//        return 0;
//    }
//
//    // Initialize cycle storage
//    CycleList cycleList;
//    initCycleList(&cycleList);
//
//    // Step 1: Construct a weighted adjacency matrix
//    GraphInterface* gi = ctx->graph_interface;
//    int** adj_matrix = (int**)malloc(vertices * sizeof(int*));
//    for (int i = 0; i < vertices; i++) {
//        adj_matrix[i] = (int*)calloc(vertices, sizeof(int));
//    }
//
//    for (int i = 0; i < vertices; i++) {
//        int out_degree;
//        int* neighbors = gi->get_all_edges_from_vertex(gi, i, &out_degree);
//        for (int j = 0; j < out_degree; j++) {
//            adj_matrix[i][neighbors[j]] = gi->get_edge(gi, i, neighbors[j]);
//        }
//        free(neighbors);
//    }
//
//    // Step 2: Compute Maximum Weight Matching using a better algorithm
//    int* match = (int*)malloc(vertices * sizeof(int));
//    for (int i = 0; i < vertices; i++) match[i] = -1;
//
//    // This function should use a **better algorithm**, currently placeholder
//    int match_count = find_maximum_weight_matching(adj_matrix, vertices, match);
//
//    // Step 3: Extract cycles from the matching structure
//    bool* visited = (bool*)calloc(vertices, sizeof(bool));
//    for (int i = 0; i < vertices; i++) {
//        if (match[i] != -1 && !visited[i]) {
//            Stack cycleStack;
//            initStack(&cycleStack, vertices);
//
//            int start = i;
//            int current = i;
//            int cycle_length = 0;
//            int* cycle = (int*)malloc(vertices * sizeof(int));
//
//            while (!visited[current] && cycle_length < vertices) {
//                visited[current] = true;
//                cycle[cycle_length++] = current;
//                push(&cycleStack, current);
//                current = match[current];
//            }
//
//            // If a cycle is detected, store it
//            if (current == start) {
//                int* final_cycle = (int*)malloc(cycle_length * sizeof(int));
//                memcpy(final_cycle, cycle, cycle_length * sizeof(int));
//                addCycle(&cycleList, final_cycle, cycle_length);
//            }
//
//            free(cycle);
//            freeStack(&cycleStack);
//        }
//    }
//
//    free(match);
//    free(visited);
//    for (int i = 0; i < vertices; i++) {
//        free(adj_matrix[i]);
//    }
//    free(adj_matrix);
//
//    *output_cycles = cycleList.cycles;
//    *cycle_sizes = cycleList.sizes;
//    *cycle_count = cycleList.count;
//
//    destroy_context(ctx);
//    return cycleList.count;
//}
//
//
//static int approximate_count_hamiltonian_cycles(void* graph, int vertices,
//    int*** output_cycles,
//    int** cycle_sizes,
//    int* cycle_count)
//{
//    GraphAlgorithmContext* ctx = create_context(graph, vertices);
//    *cycle_count = 0;
//    *output_cycles = NULL;
//    *cycle_sizes = NULL;
//    for (int start = 0; start < vertices; start++) {
//        int* path = (int*)malloc(vertices * sizeof(int));
//        path[0] = start;
//        int current = start;
//        int step = 1;
//        for (; step < vertices; step++) {
//            for (int nxt = 0; nxt < vertices; nxt++) {
//                if (ctx->graph_interface->get_edge(graph, current, nxt) > 0) {
//                    path[step] = nxt;
//                    current = nxt;
//                    break;
//                }
//            }
//        }
//        if (ctx->graph_interface->get_edge(graph, current, start) > 0) {
//            (*cycle_count)++;
//            *output_cycles = (int**)realloc(*output_cycles, (*cycle_count) * sizeof(int*));
//            *cycle_sizes = (int*)realloc(*cycle_sizes, (*cycle_count) * sizeof(int));
//            (*output_cycles)[*cycle_count - 1] = path;
//            (*cycle_sizes)[*cycle_count - 1] = vertices;
//        }
//        else {
//            free(path);
//        }
//    }
//    destroy_context(ctx);
//    return *cycle_count;
//}

/*-----------------------------------------------------------------------------
 * Wrapper to get graph size from the interface
 *---------------------------------------------------------------------------*/
static int calculate_size_wrapper(void* graph) {
    if (!graph) return -1;
    GraphInterface* gi = (GraphInterface*)graph;
    if (!gi->calculate_size) return -1;
    return gi->calculate_size(graph);
}

/*-----------------------------------------------------------------------------
 * Checking if there's at least 1 Hamiltonian cycle (used in minimal extension)
 *---------------------------------------------------------------------------*/
//static void checkCycle(GraphAlgorithmContext* context, int start, int current, int depth,
//    int* path, bool* visited, bool* foundOne);

//static bool hasHamiltonianCycle(GraphAlgorithmContext* context) {
//    bool foundOne = false;
//    for (int i = 0; i < context->vertices && !foundOne; i++) {
//        int* path = (int*)malloc(context->vertices * sizeof(int));
//        bool* visited = (bool*)calloc(context->vertices, sizeof(bool));
//        if (!path || !visited) {
//            free(path);
//            free(visited);
//            continue;
//        }
//        checkCycle(context, i, i, 1, path, visited, &foundOne);
//        free(path);
//        free(visited);
//    }
//    return foundOne;
//}
//static void checkCycle(GraphAlgorithmContext* context, int start, int current, int depth,
//    int* path, bool* visited, bool* foundOne)
//{
//    if (*foundOne) return;
//    path[depth - 1] = current;
//    visited[current] = true;
//
//    GraphInterface* gi = context->graph_interface;
//    if (depth == context->vertices) {
//        int w = gi->get_edge(gi, current, start);
//        if (w > 0) {
//            *foundOne = true;
//        }
//    }
//    else {
//        for (int nxt = 0; nxt < context->vertices && !(*foundOne); nxt++) {
//            int w2 = gi->get_edge(gi, current, nxt);
//            if (w2 > 0 && !visited[nxt]) {
//                checkCycle(context, start, nxt, depth + 1, path, visited, foundOne);
//            }
//        }
//    }
//    visited[current] = false;
//}

/*-----------------------------------------------------------------------------
 * explore_extensions: for minimal extension search
 *---------------------------------------------------------------------------*/
//static void explore_extensions(GraphAlgorithmContext* context, int vertices, int addedEdges, int* minEdgesNeeded);
//static void explore_extensions(GraphAlgorithmContext* context, int vertices, int addedEdges, int* minEdgesNeeded) {
//    if (addedEdges >= *minEdgesNeeded) return;
//    if (hasHamiltonianCycle(context)) {
//        if (addedEdges < *minEdgesNeeded) {
//            *minEdgesNeeded = addedEdges;
//        }
//        return;
//    }
//    GraphInterface* gi = context->graph_interface;
//    /* Try adding edges that do not exist. */
//    for (int i = 0; i < vertices; i++) {
//        for (int j = 0; j < vertices; j++) {
//            if (i != j) {
//                int w = gi->get_edge(gi, i, j);
//                if (w == 0) {
//                    gi->add_edge(gi, i, j, 1);
//                    explore_extensions(context, vertices, addedEdges + 1, minEdgesNeeded);
//                    gi->add_edge(gi, i, j, -1); /* remove that edge again */
//                }
//            }
//        }
//    }
//}

/*-----------------------------------------------------------------------------
 * GraphAlgorithm structure
 *---------------------------------------------------------------------------*/
typedef struct GraphAlgorithmTag {
    int  (*calculate_size)(void* graph);
    int  (*find_cycles)(void* graph, int vertices,
        int*** output_cycles, int** cycle_sizes, int* cycle_count);
    int  (*count_hamiltonian_cycles)(void* graph, int vertices,
        int*** output_cycles, int** cycle_sizes, int* cycle_count);
    void (*calculate_metric)(void* g1, int v1, void* g2, int v2,
        int* exact_metric, int* approximate_metric);
    int  (*find_minimal_extension)(void* graph, int vertices);
    int  (*count_maximal_cycles)(void* graph, int vertices);
    int  (*find_maximal_cycles)(void* graph, int vertices,
        int*** output_cycles, int** cycle_sizes, int* cycle_count);
} GraphAlgorithm;

/*
 * Forward declarations of wrappers
 */
static int  find_cycles_wrapper(
    void* graph, int vertices,
    int*** outA, int** outS, int* outCount);
static int  count_hamiltonian_cycles_wrapper(
    void* graph, int vertices,
    int*** outA, int** outS, int* outCount);
static void calculate_metric_wrapper(
    void* g1, int v1, void* g2, int v2,
    int* exact_metric, int* approximate_metric);
static int  find_minimal_extension_wrapper(void* graph, int v);
static int  count_maximal_cycles_wrapper(void* graph, int v);
static int  find_maximal_cycles_wrapper(
    void* graph, int v,
    int*** outA, int** outS, int* outC);

/* Our global "default_algorithm" object */
static GraphAlgorithm default_algorithm = {
    .calculate_size = calculate_size_wrapper,
    .find_cycles = find_cycles_wrapper,
    .count_hamiltonian_cycles = count_hamiltonian_cycles_wrapper,
    .calculate_metric = calculate_metric_wrapper,
    .find_minimal_extension = find_minimal_extension_wrapper,
    .count_maximal_cycles = count_maximal_cycles_wrapper,
    .find_maximal_cycles = find_maximal_cycles_wrapper
};

/*
 * EXACT: find_cycles_wrapper
 *   - enumerates all simple cycles of length >= 2 edges
 */
#include <windows.h>  

static int find_cycles_wrapper(
    void* graph,
    int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count)
{
    if (vertices >= THRESHOLD) {
        printf("Graph size exceeds threshold, using approximate algorithm.\n");
        return approximate_find_cycles(graph, vertices, output_cycles, cycle_sizes, cycle_count);
    }

    LARGE_INTEGER frequency, start_exact, end_exact, start_approx, end_approx;
    QueryPerformanceFrequency(&frequency); 

    QueryPerformanceCounter(&start_exact);

    GraphAlgorithmContext* ctx = create_context(graph, vertices);
    if (!ctx) return 0;

    CycleList cycleList;
    initCycleList(&cycleList);

    StringList uniqueCycles;
    initStringList(&uniqueCycles);

    int localCount = 0;

    for (int start = 0; start < vertices; start++) {
        /* Use a stack for DFS */
        Stack stack;
        initStack(&stack, vertices);

        bool* visited = (bool*)calloc(vertices, sizeof(bool));
        if (!visited) {
            freeStack(&stack);
            continue;
        }

        /* Ensure all vertices and paths are checked */
        int out_degree;
        int* neighbors = ctx->graph_interface->get_all_edges_from_vertex(ctx->graph_interface, start, &out_degree);

        if (out_degree > 0) {
            dfs_findCycles(ctx, start, start, &stack, visited, &uniqueCycles, &cycleList, &localCount);
        }

        free(neighbors);
        freeStack(&stack);
        free(visited);
    }

    QueryPerformanceCounter(&end_exact);  // End timing
    double exact_time = (double)(end_exact.QuadPart - start_exact.QuadPart) * 1000000.0 / frequency.QuadPart;

    printf("Time taken for exact cycle detection: %.2f microseconds\n", exact_time);

    // Run approximate cycle detection algorithm
    QueryPerformanceCounter(&start_approx);  // Start timing
    int approx_cycle_count;
    int** approx_output_cycles;
    int* approx_cycle_sizes;

    approx_cycle_count = approximate_find_cycles(graph, vertices, &approx_output_cycles, &approx_cycle_sizes, &approx_cycle_count);
    QueryPerformanceCounter(&end_approx);  // End timing

    double approx_time = (double)(end_approx.QuadPart - start_approx.QuadPart) * 1000000.0 / frequency.QuadPart;

    printf("Approximate cycle count: %d\n", approx_cycle_count);
    printf("Time taken for approximate cycle detection: %.2f microseconds\n", approx_time);

    *output_cycles = cycleList.cycles;
    *cycle_sizes = cycleList.sizes;
    *cycle_count = cycleList.count;

    freeStringList(&uniqueCycles);
    destroy_context(ctx);

    return *cycle_count;
}




/*
 * EXACT: count_hamiltonian_cycles_wrapper
 */
static int count_hamiltonian_cycles_wrapper(
    void* graph,
    int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count)
{
    if (vertices >= THRESHOLD) {
        return approximate_count_hamiltonian_cycles(graph, vertices,
            output_cycles, cycle_sizes, cycle_count);
    }
    GraphAlgorithmContext* ctx = create_context(graph, vertices);
    if (!ctx) return 0;

    CycleList  cycleList;
    initCycleList(&cycleList);

    StringList uniqueCycles;
    initStringList(&uniqueCycles);

    int localCount = 0;
    for (int start = 0; start < vertices; start++) {
        Stack st;
        initStack(&st, vertices);

        bool* visited = (bool*)calloc(vertices, sizeof(bool));
        if (!visited) {
            freeStack(&st);
            continue;
        }
        /* Perform Hamiltonian DFS from 'start'. */
        backtrack_hamiltonian(ctx, start, start, &st, visited,
            &uniqueCycles, &cycleList, &localCount);

        freeStack(&st);
        free(visited);
    }

    *output_cycles = cycleList.cycles;
    *cycle_sizes = cycleList.sizes;
    *cycle_count = cycleList.count;

    freeStringList(&uniqueCycles);
    destroy_context(ctx);
    return cycleList.count;
}

/*
 * EXACT: metric calculation
 */
//static int calculate_required_operations(
//    GraphAlgorithmContext* context1,
//    GraphAlgorithmContext* context2,
//    int* arr,
//    int n,
//    int smaller_n
//)
//{
//    int required_operations = n - smaller_n;
//    GraphInterface* g1 = context1->graph_interface;
//    GraphInterface* g2 = context2->graph_interface;
//
//    for (int i = 0; i < n; i++) {
//        for (int j = 0; j < n; j++) {
//            if (i >= smaller_n || j >= smaller_n) {
//                required_operations += g1->get_edge(g1, arr[i] - 1, arr[j] - 1);
//            }
//            else {
//                int e1 = g1->get_edge(g1, arr[i] - 1, arr[j] - 1);
//                int e2 = g2->get_edge(g2, i, j);
//                required_operations += abs(e1 - e2);
//            }
//        }
//    }
//    return required_operations;
//}
//static int calculate_graph_metric(
//    GraphAlgorithmContext* context1,
//    GraphAlgorithmContext* context2,
//    int vertices1,
//    int vertices2
//)
//{
//    if (vertices1 > THRESHOLD || vertices2 > THRESHOLD) {
//        printf("Using approximate algorithm for large graphs.\n");
//        return approximate_calculate_metric(context1, context2, vertices1, vertices2);
//    }
//
//    int* arr = (int*)malloc(vertices1 * sizeof(int));
//    for (int i = 0; i < vertices1; i++) {
//        arr[i] = i + 1;
//    }
//    int minOperations = INT_MAX;
//    do {
//        int tmp = calculate_required_operations(context1, context2, arr, vertices1, vertices2);
//        minOperations = common_min(minOperations, tmp);
//    } while (nextPermutation(arr, vertices1));
//
//    free(arr);
//    return minOperations;
//}
static void calculate_metric_wrapper(
    void* g1, int v1,
    void* g2, int v2,
    int* exact_metric,
    int* approximate_metric)
{
    /* Ensure g1 has >= #vertices. If not, swap. */
    if (common_max(v1, v2) == v2) {
        void* tmp = g1;  g1 = g2;   g2 = tmp;
        int   tv = v1;  v1 = v2;   v2 = tv;
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

/*
 * EXACT: find_minimal_extension_wrapper
 */
static int find_minimal_extension_wrapper(void* graph, int vertices) {
    if (vertices >= THRESHOLD) {
        return approximate_find_minimal_extension(graph, vertices);
    }
    GraphAlgorithmContext* ctx = create_context(graph, vertices);
    if (!ctx) return 0;

    bool foundOne = hasHamiltonianCycle(ctx);
    if (foundOne) {
        destroy_context(ctx);
        return 0;
    }

    int minEdgesNeeded = INT_MAX;
    explore_extensions(ctx, vertices, 0, &minEdgesNeeded);
    destroy_context(ctx);

    if (minEdgesNeeded == INT_MAX) return 0;
    return minEdgesNeeded;
}

/*
 * EXACT: count_maximal_cycles_wrapper => length of the longest simple cycle
 */
static int count_maximal_cycles_wrapper(void* graph, int vertices) {
    if (vertices >= THRESHOLD) {
        return approximate_count_maximal_cycles(graph, vertices);
    }
    GraphAlgorithmContext* ctx = create_context(graph, vertices);
    if (!ctx) return 0;

    int maxCycleLength = 0;
    for (int start = 0; start < vertices; start++) {
        Stack st;
        initStack(&st, vertices);

        bool* visited = (bool*)calloc(vertices, sizeof(bool));
        if (visited) {
            dfs_maxCycleLength(ctx, start, start, &st, visited, &maxCycleLength);
        }
        free(visited);
        freeStack(&st);
    }
    destroy_context(ctx);
    return maxCycleLength;
}

/*
 * EXACT: find_maximal_cycles_wrapper => enumerates all cycles that match
 * the maximum cycle length found above.
 */
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

    /* First: find the maximum cycle length. */
    int maxCycleLength = 0;
    for (int start = 0; start < vertices; start++) {
        Stack st;
        initStack(&st, vertices);

        bool* visited = (bool*)calloc(vertices, sizeof(bool));
        if (visited) {
            dfs_maxCycleLength(ctx, start, start, &st, visited, &maxCycleLength);
        }
        free(visited);
        freeStack(&st);
    }

    /* Next: collect all cycles of length == maxCycleLength. */
    CycleList cycleList;
    initCycleList(&cycleList);

    StringList uniqueCycles;
    initStringList(&uniqueCycles);

    int foundCount = 0;
    for (int start = 0; start < vertices; start++) {
        Stack st;
        initStack(&st, vertices);

        bool* visited = (bool*)calloc(vertices, sizeof(bool));
        if (!visited) {
            freeStack(&st);
            continue;
        }
        dfs_findMaxCycles(ctx, start, start, &st, visited,
            &uniqueCycles, &cycleList,
            &maxCycleLength, &foundCount);

        freeStack(&st);
        free(visited);
    }

    *output_cycles = cycleList.cycles;
    *cycle_sizes = cycleList.sizes;
    *cycle_count = cycleList.count;

    freeStringList(&uniqueCycles);
    destroy_context(ctx);

    return cycleList.count;
}

/*-----------------------------------------------------------------------------
 * "print_cycles": prints cycles as "0 -> 1 -> 2 -> 0", etc.
 *---------------------------------------------------------------------------*/
//void print_cycles(GraphInterface* multigraph, int** output_cycles, int* cycle_sizes, int cycle_count) {
//    for (int i = 0; i < cycle_count; i++) {
//        for (int j = 0; j < cycle_sizes[i]; j++) {
//            printf("%d", output_cycles[i][j]);
//            if (j < cycle_sizes[i] - 1) {
//                printf(" -> ");
//            }
//        }
//        printf("\n");
//        /* Free each cycle array after printing. */
//        free(output_cycles[i]);
//    }
//    free(output_cycles);
//    free(cycle_sizes);
//}

/*-----------------------------------------------------------------------------
 * GraphArray: hold multiple graphs
 *---------------------------------------------------------------------------*/
//void init_graph_array(GraphArray* arr, size_t capacity) {
//    arr->data = (GraphInterface**)malloc(capacity * sizeof(GraphInterface*));
//    arr->size = 0;
//    arr->capacity = capacity;
//}
//void add_graph(GraphArray* arr, GraphInterface* graph) {
//    if (arr->size >= arr->capacity) {
//        arr->capacity *= 2;
//        arr->data = (GraphInterface**)realloc(arr->data, arr->capacity * sizeof(GraphInterface*));
//    }
//    arr->data[arr->size++] = graph;
//}
//void free_graph_array(GraphArray* arr) {
//    free(arr->data);
//}

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
 *    1) All cycles
 *    2) Hamiltonian cycles
 *    3) Minimal extension for Hamiltonian cycle
 *    4) Maximal cycle length
 *    5) All cycles of that maximal length
 */
extern GraphAlgorithm default_algorithm;  /* Our global struct above */

void analyze_multigraph(GraphInterface* multigraph) {
    printf("Analyzing Multigraph:\n");
    printf("------------------------------------------------\n");

    int graph_size = default_algorithm.calculate_size(multigraph);
    printf("Graph size: %d\n", graph_size);

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

        GraphInterface* multigraph = create_multigraph(vertices);
        if (!multigraph) {
            fprintf(stderr, "Failed to initialize graph interface for graph %d.\n", i + 1);
            continue;
        }

        if (i == 0) {
            add_graph(multigraphs_to_compare, multigraph);
        }

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

/* Compare two loaded graphs for metrics */
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
    printf("Graph similarity metric (exact): %d\n", exact_metric);
    printf("Graph similarity metric (approx): %d\n", approximate_metric);
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
            process_metrics(multigraphs_to_compare.data[0],
                multigraphs_to_compare.data[1]);
        }
    }

    free_graph_array(&multigraphs_to_compare);
    return EXIT_SUCCESS;
}
