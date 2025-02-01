#include "approximation_algorithm.h"
#include "common_utils.h"
#include "graph_interface.h"
#include <time.h>

#define MAX_ITER 20000
#define INITIAL_TEMP 200.0
#define COOLING_RATE 0.99

int approximate_find_cycles(void* graph, int vertices, int*** output_cycles, int** cycle_sizes, int* cycle_count) {
    GraphAlgorithmContext* context = create_context(graph, vertices);
    *cycle_count = 0;
    *output_cycles = NULL;
    *cycle_sizes = NULL;

    for (int i = 0; i < vertices / 2; i++) {
        for (int j = 0; j < vertices / 2; j++) {
            if (context->graph_interface->get_edge(graph, i, j) > 0) {
                *cycle_count += 1;
                *output_cycles = (int**)realloc(*output_cycles, *cycle_count * sizeof(int*));
                *cycle_sizes = (int*)realloc(*cycle_sizes, *cycle_count * sizeof(int));

                int* cycle = (int*)malloc(3 * sizeof(int));
                cycle[0] = i;
                cycle[1] = j;
                cycle[2] = i;

                (*output_cycles)[*cycle_count - 1] = cycle;
                (*cycle_sizes)[*cycle_count - 1] = 3;
            }
        }
    }

    destroy_context(context);
    return *cycle_count;
}

// Function to count Hamiltonian cycles
int approximate_count_hamiltonian_cycles(void* graph, int vertices, int*** output_cycles, int** cycle_sizes, int* cycle_count) {
    GraphAlgorithmContext* context = create_context(graph, vertices);
    *cycle_count = 0;
    *output_cycles = NULL;
    *cycle_sizes = NULL;

    for (int start = 0; start < vertices; start++) {
        int* path = (int*)malloc(vertices * sizeof(int));
        path[0] = start;
        int current = start;
        int step = 1;

        for (; step < vertices; step++) {
            for (int next = 0; next < vertices; next++) {
                if (context->graph_interface->get_edge(graph, current, next) > 0) {
                    path[step] = next;
                    current = next;
                    break;
                }
            }
        }

        if (context->graph_interface->get_edge(graph, current, start) > 0) {
            *cycle_count += 1;
            *output_cycles = (int**)realloc(*output_cycles, *cycle_count * sizeof(int*));
            *cycle_sizes = (int*)realloc(*cycle_sizes, *cycle_count * sizeof(int));

            (*output_cycles)[*cycle_count - 1] = path;
            (*cycle_sizes)[*cycle_count - 1] = vertices;
        }
        else {
            free(path);
        }
    }

    destroy_context(context);
    return *cycle_count;
}

// Function to generate a random permutation
void generate_random_permutation(int* arr, int n) {
    for (int i = 0; i < n; i++) {
        int j = rand() % n;
        common_swap(&arr[i], &arr[j]);
    }
}

// Function to approximate required operations
static int approximate_required_operations(GraphAlgorithmContext* context_1, GraphAlgorithmContext* context_2, int* arr, int n, int smaller_n) {
    int required_operations = n - smaller_n;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i >= smaller_n || j >= smaller_n) {
                required_operations += context_1->graph_interface->get_edge(context_1->graph_interface, arr[i] - 1, arr[j] - 1);
                continue;
            }

            int edge_1 = context_1->graph_interface->get_edge(context_1->graph_interface, arr[i] - 1, arr[j] - 1);
            int edge_2 = context_2->graph_interface->get_edge(context_2->graph_interface, i, j);
            required_operations += abs(edge_1 - edge_2);
        }
    }

    return required_operations;
}

// Function to approximate the graph similarity metric
int approximate_calculate_metric(GraphAlgorithmContext* context_1, GraphAlgorithmContext* context_2, int vertices_1, int vertices_2) {
    int arr[vertices_1];
    for (int i = 0; i < vertices_1; i++) {
        arr[i] = i + 1;
    }

    srand(time(NULL));

    generate_random_permutation(arr, vertices_1);
    int current_metric = approximate_required_operations(context_1, context_2, arr, vertices_1, vertices_2);
    int best_metric = current_metric;

    double temperature = INITIAL_TEMP;

    for (int iter = 0; iter < MAX_ITER; iter++) {
        int i = rand() % vertices_1;
        int j = rand() % vertices_1;
        common_swap(&arr[i], &arr[j]);

        int new_metric = approximate_required_operations(context_1, context_2, arr, vertices_1, vertices_2);

        if (new_metric < current_metric || (exp((current_metric - new_metric) / temperature) > (rand() / (double)RAND_MAX))) {
            current_metric = new_metric;
            if (new_metric < best_metric) {
                best_metric = new_metric;
            }
        }
        else {
            common_swap(&arr[i], &arr[j]);
        }

        temperature *= COOLING_RATE;
    }

    return best_metric;
}

// Function to find minimal extension for Hamiltonian cycle
int approximate_find_minimal_extension(void* graph, int vertices) {
    GraphAlgorithmContext* context = create_context(graph, vertices);
    int edge_additions = 0;

    for (int i = 0; i < vertices; i++) {
        for (int j = i + 1; j < vertices; j++) {
            if (context->graph_interface->get_edge(graph, i, j) == 0) {
                context->graph_interface->add_edge(graph, i, j, 1);
                edge_additions++;
                break;
            }
        }
    }

    destroy_context(context);
    return edge_additions;
}

// Function to count maximal cycles
int approximate_count_maximal_cycles(void* graph, int vertices) {
    GraphAlgorithmContext* context = create_context(graph, vertices);
    int max_cycle_length = 0;

    for (int i = 0; i < vertices; i++) {
        int visited[vertices];
        memset(visited, 0, sizeof(visited));

        int cycle_length = 0;
        int node = i;

        while (cycle_length < vertices) {
            visited[node] = 1;
            cycle_length++;

            int found_next = 0;
            for (int j = 0; j < vertices; j++) {
                if (!visited[j] && context->graph_interface->get_edge(graph, node, j) > 0) {
                    node = j;
                    found_next = 1;
                    break;
                }
            }

            if (!found_next) {
                break;
            }
        }

        if (cycle_length > max_cycle_length) {
            max_cycle_length = cycle_length;
        }
    }

    destroy_context(context);
    return max_cycle_length;
}

// Function to find maximal cycles
int approximate_find_maximal_cycles(void* graph, int vertices, int*** output_cycles, int** cycle_sizes, int* cycle_count) {
    GraphAlgorithmContext* context = create_context(graph, vertices);
    *cycle_count = 0;
    *output_cycles = NULL;
    *cycle_sizes = NULL;

    for (int i = 0; i < vertices; i++) {
        int* cycle = (int*)malloc(vertices * sizeof(int));
        int length = 0;
        int visited[vertices];
        memset(visited, 0, sizeof(visited));

        int node = i;
        while (length < vertices) {
            visited[node] = 1;
            cycle[length++] = node;

            int found_next = 0;
            for (int j = 0; j < vertices; j++) {
                if (!visited[j] && context->graph_interface->get_edge(graph, node, j) > 0) {
                    node = j;
                    found_next = 1;
                    break;
                }
            }
            if (!found_next) break;
        }

        *cycle_count += 1;
        *output_cycles = (int**)realloc(*output_cycles, *cycle_count * sizeof(int*));
        *cycle_sizes = (int*)realloc(*cycle_sizes, *cycle_count * sizeof(int));
        (*output_cycles)[*cycle_count - 1] = cycle;
        (*cycle_sizes)[*cycle_count - 1] = length;
    }

    destroy_context(context);
    return *cycle_count;
}
