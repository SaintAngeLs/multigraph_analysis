#include "approximation_algorithm.h"

#include "common_utils.h"


int approximate_find_cycles(void *graph, int vertices, GArray *output_cycles) {
    GraphAlgorithmContext *context = create_context(graph, vertices);
    int approximate_cycle_count = 0;

    for (int i = 0; i < vertices / 2; i++) {
        for (int j = 0; j < vertices / 2; j++) {
            if (context->graph_interface->get_edge(graph, i, j) > 0) {
                GArray *cycle = g_array_new(FALSE, FALSE, sizeof(int));
                g_array_append_val(cycle, i);
                g_array_append_val(cycle, j);
                g_array_append_val(cycle, i);
                g_array_append_val(output_cycles, cycle);
                approximate_cycle_count++;
            }
        }
    }

    destroy_context(context);
    return approximate_cycle_count;
}

int approximate_count_hamiltonian_cycles(void *graph, int vertices, GArray *output_cycles) {
    GraphAlgorithmContext *context = create_context(graph, vertices);
    int approximate_count = 0;

    for (int start = 0; start < vertices; start++) {
        GArray *path = g_array_new(FALSE, FALSE, sizeof(int));
        g_array_append_val(path, start);

        int current = start;
        for (int step = 0; step < vertices - 1; step++) {
            for (int next = 0; next < vertices; next++) {
                if (context->graph_interface->get_edge(graph, current, next) > 0) {
                    g_array_append_val(path, next);
                    current = next;
                    break;
                }
            }
        }

        if (context->graph_interface->get_edge(graph, current, start) > 0) {
            g_array_append_val(output_cycles, path);
            approximate_count++;
        } else {
            g_array_free(path, TRUE);
        }
    }

    destroy_context(context);
    return approximate_count;
}


static int approximate_required_operations(GraphAlgorithmContext *context_1, GraphAlgorithmContext *context_2, int *arr, int n, int smaller_n) {
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

int approximate_calculate_metric(void *graph_1, int vertices_1, void *graph_2, int vertices_2) {
    GraphAlgorithmContext *context_1 = create_context(graph_1, vertices_1);
    GraphAlgorithmContext *context_2 = create_context(graph_2, vertices_2);

    int arr[vertices_1];
    for (int i = 0; i < vertices_1; i++) {
        arr[i] = i + 1;
    }

    int min_operations = INT_MAX;
    const int NUM_SAMPLES = 100;

    for (int sample = 0; sample < NUM_SAMPLES; sample++) {
        for (int i = vertices_1 - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            swap(&arr[i], &arr[j]);
        }

        int tmp = approximate_required_operations(context_1, context_2, arr, vertices_1, vertices_2);
        min_operations = min(min_operations, tmp);
    }

    destroy_context(context_1);
    destroy_context(context_2);

    return min_operations;
}

int approximate_find_minimal_extension(void *graph, int vertices) {
    GraphAlgorithmContext *context = create_context(graph, vertices);
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

int approximate_count_maximal_cycles(void *graph, int vertices) {
    GraphAlgorithmContext *context = create_context(graph, vertices);
    int approximate_max_cycle_length = 0;

    // Use a simple depth-first traversal to estimate maximal cycles
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

        if (cycle_length > approximate_max_cycle_length) {
            approximate_max_cycle_length = cycle_length;
        }
    }

    destroy_context(context);
    return approximate_max_cycle_length;
}

int approximate_find_maximal_cycles(void *graph, int vertices, GArray *output_cycles) {
    GraphAlgorithmContext *context = create_context(graph, vertices);
    int approximate_max_cycle_length = 0;

    for (int i = 0; i < vertices; i++) {
        GArray *current_cycle = g_array_new(FALSE, FALSE, sizeof(int));
        int visited[vertices];
        memset(visited, 0, sizeof(visited));
        int node = i, length = 0;

        while (length < vertices) {
            visited[node] = 1;
            g_array_append_val(current_cycle, node);
            length++;

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

        if (length > approximate_max_cycle_length) {
            approximate_max_cycle_length = length;
            g_array_set_size(output_cycles, 0);
            g_array_append_val(output_cycles, current_cycle);
        } else {
            g_array_free(current_cycle, TRUE);
        }
    }

    destroy_context(context);
    return approximate_max_cycle_length;
}
