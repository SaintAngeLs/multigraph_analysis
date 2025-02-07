#include "approximation_algorithm.h"
#include "common_utils.h"
#include "graph_interface.h"
#include <time.h>
#include "matching.h"

#define MAX_ITER 20000
#define INITIAL_TEMP 200.0

int approximate_required_operations(
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
void generate_random_permutation(int* arr, int n) {
    for (int i = 0; i < n; i++) {
        int j = rand() % n;
        common_swap(&arr[i], &arr[j]);
    }
}
int approximate_calculate_metric(
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
        temperature = INITIAL_TEMP / log(iter + 2);
    }
    free(arr);
    return best_metric;
}
int approximate_find_minimal_extension(void* graph, int vertices) {
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
#define ITERATIONS 10  // Number of random DFS attempts to improve accuracy

int approximate_count_maximal_cycles(void* graph, int vertices) {
    GraphAlgorithmContext* ctx = create_context(graph, vertices);
    if (!ctx) return 0;

    GraphInterface* gi = ctx->graph_interface;
    int max_cycle_length = 0;

    srand((unsigned int)time(NULL));

    for (int iter = 0; iter < ITERATIONS; iter++) {
        for (int start = 0; start < vertices; start++) {
            
            int* visited = (int*)calloc(vertices, sizeof(int));
            int* path = (int*)malloc(vertices * sizeof(int));
            int cycle_length = 0;
            int node = start;

            while (cycle_length < vertices) {
                visited[node] = 1;
                path[cycle_length++] = node;

                int out_degree = 0;
                int* neighbors = gi->get_all_edges_from_vertex(gi, node, &out_degree);
                if (out_degree == 0) {
                    free(neighbors);
                    break;
                }

                // Wybieramy tylko te sąsiedztwa, które nie są odwiedzone.
                int possible_moves = 0;
                int* possible = (int*)malloc(out_degree * sizeof(int));
                for (int j = 0; j < out_degree; j++) {
                    if (!visited[neighbors[j]]) {
                        possible[possible_moves++] = neighbors[j];
                    }
                }
                free(neighbors);

                if (possible_moves == 0) {
                    free(possible);
                    break;
                }

                // Możemy zastosować priorytet wyboru na podstawie out-degree.
                // Wybieramy sąsiada o największej liczbie wychodzących krawędzi.
                int best_choice = -1;
                int max_degree = -1;
                for (int k = 0; k < possible_moves; k++) {
                    int degree = gi->get_out_degree(gi, possible[k]);
                    if (degree > max_degree) {
                        max_degree = degree;
                        best_choice = possible[k];
                    }
                }
                free(possible);
                if (best_choice == -1) break;
                node = best_choice;
            }

            if (gi->get_edge(gi, node, start) > 0 && cycle_length > 1) {
                cycle_length++;  
                if (cycle_length > max_cycle_length) {
                    max_cycle_length = cycle_length;
                }
            }

            free(visited);
            free(path);
        }
    }

    destroy_context(ctx);
    return max_cycle_length;
}


int approximate_find_maximal_cycles(void* graph, int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count)
{
    GraphAlgorithmContext* ctx = create_context(graph, vertices);
    *cycle_count = 0;
    *output_cycles = NULL;
    *cycle_sizes = NULL;

    // Seed the random number generator.
    srand((unsigned int)time(NULL));

    for (int i = 0; i < vertices; i++) {
        // Allocate space for up to (vertices + 1) elementów (na zamknięty cykl).
        int* cycle = (int*)malloc((vertices + 1) * sizeof(int));
        int length = 0;
        int* visited = (int*)calloc(vertices, sizeof(int));

        int node = i;
        while (length < vertices) {
            visited[node] = 1;
            cycle[length++] = node;

            int out_degree = 0;
            int* neighbor_list = ctx->graph_interface->get_all_edges_from_vertex(ctx->graph_interface, node, &out_degree);
            if (out_degree == 0) {
                free(neighbor_list);
                break;
            }

            for (int k = out_degree - 1; k > 0; k--) {
                int r = rand() % (k + 1);
                int temp = neighbor_list[k];
                neighbor_list[k] = neighbor_list[r];
                neighbor_list[r] = temp;
            }

            int found_next = 0;
            for (int idx = 0; idx < out_degree; idx++) {
                int j = neighbor_list[idx];
                if (!visited[j]) {
                    node = j;
                    found_next = 1;
                    break;
                }
            }
            free(neighbor_list);
            if (!found_next)
                break;
        }

        // Próba domknięcia cyklu – jeśli długość > 1 i istnieje krawędź z ostatniego wierzchołka do startu.
        if (length > 1 && ctx->graph_interface->get_edge(ctx->graph_interface, node, i) > 0) {
            cycle[length++] = i;  // Zamknięcie cyklu.
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


/*
    Approximation algorithms for cycle and path partitions in complete graphs https://arxiv.org/pdf/2311.11332
*/
int approximate_find_cycles(void* graph, int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count)
{
    GraphAlgorithmContext* ctx = create_context(graph, vertices);
    if (!ctx) {
        *output_cycles = NULL;
        *cycle_sizes = NULL;
        *cycle_count = 0;
        return 0;
    }

    // Initialize cycle storage
    CycleList cycleList;
    initCycleList(&cycleList);

    // Step 1: Construct a weighted adjacency matrix
    GraphInterface* gi = ctx->graph_interface;
    int** adj_matrix = (int**)malloc(vertices * sizeof(int*));
    for (int i = 0; i < vertices; i++) {
        adj_matrix[i] = (int*)calloc(vertices, sizeof(int));
    }

    for (int i = 0; i < vertices; i++) {
        int out_degree;
        int* neighbors = gi->get_all_edges_from_vertex(gi, i, &out_degree);
        for (int j = 0; j < out_degree; j++) {
            adj_matrix[i][neighbors[j]] = gi->get_edge(gi, i, neighbors[j]);
        }
        free(neighbors);
    }

    // Step 2: Compute Maximum Weight Matching using a better algorithm
    int* match = (int*)malloc(vertices * sizeof(int));
    for (int i = 0; i < vertices; i++) match[i] = -1;

    // This function should use a **better algorithm**, currently placeholder
    int match_count = find_maximum_weight_matching(adj_matrix, vertices, match);

    // Step 3: Extract cycles from the matching structure
    bool* visited = (bool*)calloc(vertices, sizeof(bool));
    for (int i = 0; i < vertices; i++) {
        if (match[i] != -1 && !visited[i]) {
            Stack cycleStack;
            initStack(&cycleStack, vertices);

            int start = i;
            int current = i;
            int cycle_length = 0;
            int* cycle = (int*)malloc(vertices * sizeof(int));

            while (!visited[current] && cycle_length < vertices) {
                visited[current] = true;
                cycle[cycle_length++] = current;
                push(&cycleStack, current);
                current = match[current];
            }

            // If a cycle is detected, store it
            if (current == start) {
                int* final_cycle = (int*)malloc(cycle_length * sizeof(int));
                memcpy(final_cycle, cycle, cycle_length * sizeof(int));
                addCycle(&cycleList, final_cycle, cycle_length);
            }

            free(cycle);
            freeStack(&cycleStack);
        }
    }

    free(match);
    free(visited);
    for (int i = 0; i < vertices; i++) {
        free(adj_matrix[i]);
    }
    free(adj_matrix);

    *output_cycles = cycleList.cycles;
    *cycle_sizes = cycleList.sizes;
    *cycle_count = cycleList.count;

    destroy_context(ctx);
    return cycleList.count;
}


int approximate_count_hamiltonian_cycles(void* graph, int vertices,
    int*** output_cycles,
    int** cycle_sizes,
    int* cycle_count)
{
    GraphAlgorithmContext* ctx = create_context(graph, vertices);
    *cycle_count = 0;
    *output_cycles = NULL;
    *cycle_sizes = NULL;

    int* path = (int*)malloc((vertices + 1) * sizeof(int)); // Allocate for extra closing vertex
    bool* visited = (bool*)calloc(vertices, sizeof(bool));

    for (int start = 0; start < vertices; start++) {
        path[0] = start;
        visited[start] = true;
        int step = 1;
        int current = start;

        for (; step < vertices; step++) {
            for (int nxt = 0; nxt < vertices; nxt++) {
                if (!visited[nxt] && ctx->graph_interface->get_edge(graph, current, nxt) > 0) {
                    path[step] = nxt;
                    visited[nxt] = true;
                    current = nxt;
                    break;
                }
            }
        }

        if (ctx->graph_interface->get_edge(graph, current, start) > 0) {
            path[step] = start;  // Close the cycle
            step++;  // Increment length to account for closure

            (*cycle_count)++;
            *output_cycles = (int**)realloc(*output_cycles, (*cycle_count) * sizeof(int*));
            *cycle_sizes = (int*)realloc(*cycle_sizes, (*cycle_count) * sizeof(int));
            (*output_cycles)[*cycle_count - 1] = (int*)malloc(step * sizeof(int));
            memcpy((*output_cycles)[*cycle_count - 1], path, step * sizeof(int));
            (*cycle_sizes)[*cycle_count - 1] = step;
        }

        memset(visited, false, vertices * sizeof(bool)); // Reset visited array
    }

    free(path);
    free(visited);
    destroy_context(ctx);
    return *cycle_count;
}