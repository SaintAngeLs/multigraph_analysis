#include "method_wrapper.h"


int calculate_size_wrapper(void* graph) {
    if (!graph) return -1;
    GraphInterface* gi = (GraphInterface*)graph;
    if (!gi->calculate_size) return -1;
    return gi->calculate_size(graph);
}


int find_cycles_wrapper(
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

int count_hamiltonian_cycles_wrapper(
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

void calculate_metric_wrapper(
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
int find_minimal_extension_wrapper(void* graph, int vertices) {
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
int count_maximal_cycles_wrapper(void* graph, int vertices) {
    LARGE_INTEGER frequency, start_exact, end_exact, start_approx, end_approx;
    QueryPerformanceFrequency(&frequency);

    if (vertices >= THRESHOLD) {
        return approximate_count_maximal_cycles(graph, vertices);
    }
    GraphAlgorithmContext* ctx = create_context(graph, vertices);
    if (!ctx) return 0;

    QueryPerformanceCounter(&start_exact);

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

    QueryPerformanceCounter(&end_exact);
    double exact_time = (double)(end_exact.QuadPart - start_exact.QuadPart) * 1000000.0 / frequency.QuadPart;
    printf("Time taken for exact maximal cycle length detection: %.2f microseconds\n", exact_time);

    // Measure approximate cycle detection time
    QueryPerformanceCounter(&start_approx);
    int approx_max_cycle_length = approximate_count_maximal_cycles(graph, vertices);
    QueryPerformanceCounter(&end_approx);

    double approx_time = (double)(end_approx.QuadPart - start_approx.QuadPart) * 1000000.0 / frequency.QuadPart;
    printf("Approximate maximal cycle length: %d\n", approx_max_cycle_length);
    printf("Time taken for approximate maximal cycle length detection: %.2f microseconds\n", approx_time);


    destroy_context(ctx);
    return maxCycleLength;
}

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
)
{
    LARGE_INTEGER frequency, start_exact, end_exact, start_approx, end_approx;
    QueryPerformanceFrequency(&frequency);

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

    QueryPerformanceCounter(&start_exact);
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

    QueryPerformanceCounter(&end_exact);
    double exact_time = (double)(end_exact.QuadPart - start_exact.QuadPart) * 1000000.0 / frequency.QuadPart;
    printf("Time taken for exact maximal cycle detection: %.2f microseconds\n", exact_time);

    // Measure approximate cycle detection time
    QueryPerformanceCounter(&start_approx);
    int approx_cycle_count;
    int** approx_output_cycles;
    int* approx_cycle_sizes;

    approx_cycle_count = approximate_find_maximal_cycles(graph, vertices, &approx_output_cycles, &approx_cycle_sizes, &approx_cycle_count);
    QueryPerformanceCounter(&end_approx);

    double approx_time = (double)(end_approx.QuadPart - start_approx.QuadPart) * 1000000.0 / frequency.QuadPart;
    printf("Approximate maximal cycle count: %d\n", approx_cycle_count);
    printf("Time taken for approximate maximal cycle detection: %.2f microseconds\n", approx_time);

    *output_cycles = cycleList.cycles;
    *cycle_sizes = cycleList.sizes;
    *cycle_count = cycleList.count;

    freeStringList(&uniqueCycles);
    destroy_context(ctx);

    return cycleList.count;
}
