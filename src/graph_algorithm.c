#include "graph_algorithm.h"

#include "common_utils.h"

#include <time.h>

GraphAlgorithmContext* create_context(void *graph, int vertices) {
    GraphAlgorithmContext *context = malloc(sizeof(GraphAlgorithmContext));
    context->graph_interface = (GraphInterface *)graph;
    context->vertices = vertices;
    return context;
}

void destroy_context(GraphAlgorithmContext *context) {
    free(context);
}

int int_cmp(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

static int calculate_size(void *graph) {
    if (!graph) {
        fprintf(stderr, "Error: Null graph pointer in calculate_size.\n");
        return -1;
    }

    GraphInterface *graph_interface = (GraphInterface *)graph;
    if (!graph_interface->calculate_size) {
        fprintf(stderr, "Error: Null calculate_size function pointer in GraphInterface.\n");
        return -1;
    }

    return graph_interface->calculate_size(graph);
}

static void normalize_cycle(GArray *cycle, char *buffer) {
    int len = cycle->len;
    int *array = (int *)cycle->data;

    int *extended_array = malloc(2 * len * sizeof(int));
    memcpy(extended_array, array, len * sizeof(int));
    memcpy(extended_array + len, array, len * sizeof(int));

    int min_idx = 0;
    for (int i = 1; i < len; i++) {
        int is_smaller = 0;
        for (int j = 0; j < len; j++) {
            if (extended_array[i + j] < extended_array[min_idx + j]) {
                is_smaller = 1;
                break;
            } else if (extended_array[i + j] > extended_array[min_idx + j]) {
                is_smaller = 0;
                break;
            }
        }
        if (is_smaller) {
            min_idx = i;
        }
    }

    int offset = 0;
    for (int i = 0; i < len; i++) {
        offset += sprintf(buffer + offset, "%d-", extended_array[min_idx + i]);
    }

    free(extended_array);
}

static int find_cycles(void *graph, int vertices, GArray *output_cycles) {
    if (vertices >= THRESHOLD) {
        return approximate_find_cycles(graph, vertices, output_cycles);
    }

    GraphAlgorithmContext *context = create_context(graph, vertices);
    int cycle_count = 0;

    GHashTable *unique_cycles = g_hash_table_new_full(g_str_hash, g_str_equal, free, NULL);

    void store_cycle(GArray *cycle) {
        GArray *stored_cycle = g_array_new(FALSE, FALSE, sizeof(int));
        g_array_append_vals(stored_cycle, cycle->data, cycle->len);
        g_array_append_val(stored_cycle, *(int *)cycle->data);
        g_array_append_val(output_cycles, stored_cycle);
    }

    void dfs(int node, int start, GArray *stack, GHashTable *visited) {
        g_array_append_val(stack, node);
        g_hash_table_add(visited, GINT_TO_POINTER(node));

        for (int i = 0; i < context->vertices; i++) {
            int edge_weight = context->graph_interface->get_edge(graph, node, i);
            if (edge_weight > 0) {
                if (i == start && (edge_weight > 1 || stack->len > 2)) {
                    char cycle_str[512];
                    normalize_cycle(stack, cycle_str);

                    if (!g_hash_table_contains(unique_cycles, cycle_str)) {
                        g_hash_table_add(unique_cycles, strdup(cycle_str));
                        cycle_count++;
                        store_cycle(stack);
                    }
                } else if (!g_hash_table_contains(visited, GINT_TO_POINTER(i))) {
                    dfs(i, start, stack, visited);
                }
            }
        }

        g_array_remove_index(stack, stack->len - 1);
        g_hash_table_remove(visited, GINT_TO_POINTER(node));
    }

    for (int i = 0; i < context->vertices; i++) {
        GArray *stack = g_array_new(FALSE, FALSE, sizeof(int));
        GHashTable *visited = g_hash_table_new(g_direct_hash, g_direct_equal);
        dfs(i, i, stack, visited);
        g_array_free(stack, TRUE);
        g_hash_table_destroy(visited);
    }

    g_hash_table_destroy(unique_cycles);
    destroy_context(context);

    return cycle_count;
}

// int approximate_find_cycles(void *graph, int vertices, GArray *output_cycles) {
//     GraphAlgorithmContext *context = create_context(graph, vertices);
//     int approximate_cycle_count = 0;

//     // Use random sampling to detect cycles
//     for (int i = 0; i < vertices / 2; i++) {
//         for (int j = 0; j < vertices / 2; j++) {
//             if (context->graph_interface->get_edge(graph, i, j) > 0) {
//                 GArray *cycle = g_array_new(FALSE, FALSE, sizeof(int));
//                 g_array_append_val(cycle, i);
//                 g_array_append_val(cycle, j);
//                 g_array_append_val(cycle, i);
//                 g_array_append_val(output_cycles, cycle);
//                 approximate_cycle_count++;
//             }
//         }
//     }

//     destroy_context(context);
//     return approximate_cycle_count;
// }

static int count_hamiltonian_cycles(void *graph, int vertices, GArray *output_cycles) {
    if (vertices >= THRESHOLD) {
        return approximate_count_hamiltonian_cycles(graph, vertices, output_cycles);
    }


    GraphAlgorithmContext *context = create_context(graph, vertices);
    int count = 0;

    GHashTable *unique_cycles = g_hash_table_new_full(g_str_hash, g_str_equal, free, NULL);

    void store_hamiltonian_cycle(GArray *cycle) {
        GArray *stored_cycle = g_array_new(FALSE, FALSE, sizeof(int));
        g_array_append_vals(stored_cycle, cycle->data, cycle->len);
        g_array_append_val(stored_cycle, g_array_index(cycle, int, 0));
        g_array_append_val(output_cycles, stored_cycle);
    }

    void backtrack(int node, int start, int depth, GArray *path, GHashTable *visited) {
        g_array_append_val(path, node);
        g_hash_table_add(visited, GINT_TO_POINTER(node));

        if (depth == context->vertices) {
            int edge_weight = context->graph_interface->get_edge(graph, node, start);
            if (edge_weight > 0 && (edge_weight > 1 || path->len > 2)) {
                char cycle_str[512];
                normalize_cycle(path, cycle_str);

                if (!g_hash_table_contains(unique_cycles, cycle_str)) {
                    g_hash_table_add(unique_cycles, strdup(cycle_str));
                    count++;
                    store_hamiltonian_cycle(path);
                }
            }
        } else {
            for (int i = 0; i < context->vertices; i++) {
                int edge_weight = context->graph_interface->get_edge(graph, node, i);
                if (edge_weight > 0 && !g_hash_table_contains(visited, GINT_TO_POINTER(i))) {
                    backtrack(i, start, depth + 1, path, visited);
                }
            }
        }

        g_array_remove_index(path, path->len - 1);
        g_hash_table_remove(visited, GINT_TO_POINTER(node));
    }

    for (int i = 0; i < context->vertices; i++) {
        GArray *path = g_array_new(FALSE, FALSE, sizeof(int));
        GHashTable *visited = g_hash_table_new(g_direct_hash, g_direct_equal);
        backtrack(i, i, 1, path, visited);
        g_array_free(path, TRUE);
        g_hash_table_destroy(visited);
    }

    g_hash_table_destroy(unique_cycles);
    destroy_context(context);

    return count;
}

// int approximate_count_hamiltonian_cycles(void *graph, int vertices, GArray *output_cycles) {
//     GraphAlgorithmContext *context = create_context(graph, vertices);
//     int approximate_count = 0;

//     // Random walks to estimate Hamiltonian cycle presence
//     for (int start = 0; start < vertices; start++) {
//         GArray *path = g_array_new(FALSE, FALSE, sizeof(int));
//         g_array_append_val(path, start);

//         int current = start;
//         for (int step = 0; step < vertices - 1; step++) {
//             for (int next = 0; next < vertices; next++) {
//                 if (context->graph_interface->get_edge(graph, current, next) > 0) {
//                     g_array_append_val(path, next);
//                     current = next;
//                     break;
//                 }
//             }
//         }

//         if (context->graph_interface->get_edge(graph, current, start) > 0) {
//             g_array_append_val(output_cycles, path);
//             approximate_count++;
//         } else {
//             g_array_free(path, TRUE);
//         }
//     }

//     destroy_context(context);
//     return approximate_count;
// }

bool nextPermutation(int *arr, int n) {
    int i = n - 2;
    // Find the first index `i` such that arr[i] < arr[i+1]
    while (i >= 0 && arr[i] >= arr[i + 1]) {
        i--;
    }

    if (i < 0) {
        return false; // No more permutations
    }

    // Find the smallest element in arr[i+1..n-1] that is greater than arr[i]
    int j = n - 1;
    while (arr[j] <= arr[i]) {
        j--;
    }

    // Swap arr[i] and arr[j]
    swap(&arr[i], &arr[j]);

    // Reverse the sequence arr[i+1..n-1]
    int left = i + 1, right = n - 1;
    while (left < right) {
        swap(&arr[left], &arr[right]);
        left++;
        right--;
    }

    return true;
}

static int calculate_required_operations(GraphAlgorithmContext *context_1, GraphAlgorithmContext *context_2, int *arr, int n, int smaller_n) {
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

static int calculate_graph_metric(GraphAlgorithmContext *context_1,  GraphAlgorithmContext *context_2, int vertices_1, int vertices_2) {
    if (vertices_1 > THRESHOLD || vertices_2 > THRESHOLD) {
        printf("Using approximate algorithm for large graphs (approximate algorithm is used twice).\n");
        return approximate_calculate_metric(context_1, context_2, vertices_1, vertices_2);
    }

    int arr[vertices_1];
    for (int i = 0; i < vertices_1; i++) {
        arr[i] = i + 1;
    }

    int min_operations = INT_MAX;
    do {
        int tmp = calculate_required_operations(context_1, context_2, arr, vertices_1, vertices_2);
        min_operations = min(min_operations, tmp);
    } while (nextPermutation(arr, vertices_1));

    return min_operations;
}

static void calculate_metric(void *graph_1, int vertices_1, void *graph_2, int vertices_2, int *exact_metric, int *approximate_metric){
    if (max(vertices_1, vertices_2) == vertices_2) {
        void *temp = graph_1;
        graph_1 = graph_2;
        graph_2 = temp;

        int temp_vertices = vertices_1;
        vertices_1 = vertices_2;
        vertices_2 = temp_vertices;
    }

    GraphAlgorithmContext *context_1 = create_context(graph_1, vertices_1);
    GraphAlgorithmContext *context_2 = create_context(graph_2, vertices_2);

    clock_t start, end;
    double cpu_time_used;

    start = clock();
    *exact_metric = calculate_graph_metric(context_1, context_2, vertices_1, vertices_2);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time taken for exact function: %f seconds\n", cpu_time_used);

    start = clock();
    *approximate_metric = approximate_calculate_metric(context_1, context_2, vertices_1, vertices_2);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time taken for approximate function: %f seconds\n", cpu_time_used);
}

static int find_minimal_extension(void *graph, int vertices) {
    if (vertices >= THRESHOLD) {
        return approximate_find_minimal_extension(graph, vertices);
    }

    GraphAlgorithmContext *context = create_context(graph, vertices);
    int min_edges_needed = INT_MAX;

    bool has_hamiltonian_cycle(void *graph, int vertices) {
        GArray *hamiltonian_cycles = g_array_new(FALSE, FALSE, sizeof(GArray *));
        int count = default_algorithm.count_hamiltonian_cycles(graph, vertices, hamiltonian_cycles);
        g_array_free(hamiltonian_cycles, TRUE);
        return count > 0;
    }

    void explore_extensions(void *graph, int added_edges) {
        if (added_edges >= min_edges_needed) {
            return;
        }

        if (has_hamiltonian_cycle(graph, vertices)) {
            min_edges_needed = added_edges;
            return;
        }

        for (int i = 0; i < vertices; i++) {
            for (int j = 0; j < vertices; j++) {
                if (i != j && context->graph_interface->get_edge(graph, i, j) == 0) {
                    context->graph_interface->add_edge(graph, i, j, 1);
                    explore_extensions(graph, added_edges + 1);

                    context->graph_interface->add_edge(graph, i, j, -1);
                }
            }
        }
    }

    explore_extensions(graph, 0);

    destroy_context(context);
    return min_edges_needed == INT_MAX ? 0 : min_edges_needed;
}

// int approximate_find_minimal_extension(void *graph, int vertices) {
//     GraphAlgorithmContext *context = create_context(graph, vertices);
//     int edge_additions = 0;

//     // Naively connect all disconnected components
//     for (int i = 0; i < vertices; i++) {
//         for (int j = i + 1; j < vertices; j++) {
//             if (context->graph_interface->get_edge(graph, i, j) == 0) {
//                 context->graph_interface->add_edge(graph, i, j, 1);
//                 edge_additions++;
//                 break;
//             }
//         }
//     }

//     destroy_context(context);
//     return edge_additions;
// }

static int count_maximal_cycles(void *graph, int vertices) {
    if (vertices >= THRESHOLD) {
        return approximate_count_maximal_cycles(graph, vertices);
    }

    GraphAlgorithmContext *context = create_context(graph, vertices);
    int max_cycle_length = 0;

    void dfs(int node, GHashTable *visited, int depth, int start) {
        g_hash_table_add(visited, GINT_TO_POINTER(node));

        for (int i = 0; i < context->vertices; i++) {
            int edge_weight = context->graph_interface->get_edge(graph, node, i);
            if (edge_weight > 0) {
                if (i == start && depth > max_cycle_length && (edge_weight > 1 || depth > 2)) {
                    max_cycle_length = depth;
                } else if (!g_hash_table_contains(visited, GINT_TO_POINTER(i))) {
                    dfs(i, visited, depth + 1, start);
                }
            }
        }

        g_hash_table_remove(visited, GINT_TO_POINTER(node));
    }

    for (int i = 0; i < context->vertices; i++) {
        GHashTable *visited = g_hash_table_new(g_direct_hash, g_direct_equal);
        dfs(i, visited, 1, i);
        g_hash_table_destroy(visited);
    }

    destroy_context(context);
    return max_cycle_length;
}

// int approximate_count_maximal_cycles(void *graph, int vertices, GArray *output_cycles) {
//     GraphAlgorithmContext *context = create_context(graph, vertices);
//     int approximate_max_cycle_length = 0;

//     // Use a simple depth-first traversal to estimate maximal cycles
//     for (int i = 0; i < vertices; i++) {
//         int visited[vertices];
//         memset(visited, 0, sizeof(visited));

//         int cycle_length = 0;
//         for (int j = 0; j < vertices; j++) {
//             if (context->graph_interface->get_edge(graph, i, j) > 0 && !visited[j]) {
//                 visited[j] = 1;
//                 cycle_length++;
//             }
//         }

//         if (cycle_length > approximate_max_cycle_length) {
//             approximate_max_cycle_length = cycle_length;
//             GArray *cycle = g_array_new(FALSE, FALSE, sizeof(int));
//             for (int k = 0; k < vertices; k++) {
//                 if (visited[k]) {
//                     g_array_append_val(cycle, k);
//                 }
//             }
//             g_array_append_val(output_cycles, cycle);
//         }
//     }

//     destroy_context(context);
//     return approximate_max_cycle_length;
// }

static int find_maximal_cycles(void *graph, int vertices, GArray *output_cycles) {
    if (vertices >= THRESHOLD) {
        return approximate_find_maximal_cycles(graph, vertices, output_cycles);
    }

    GraphAlgorithmContext *context = create_context(graph, vertices);
    int max_cycle_length = 0;
    int cycle_count = 0;

    GHashTable *unique_cycles = g_hash_table_new_full(g_str_hash, g_str_equal, free, NULL);

    void store_cycle(GArray *cycle) {
        GArray *stored_cycle = g_array_new(FALSE, FALSE, sizeof(int));
        g_array_append_vals(stored_cycle, cycle->data, cycle->len);
        g_array_append_val(stored_cycle, *(int *)cycle->data);
        g_array_append_val(output_cycles, stored_cycle);
    }

    void dfs(int node, int start, GArray *stack, GHashTable *visited) {
        g_array_append_val(stack, node);
        g_hash_table_add(visited, GINT_TO_POINTER(node));

        for (int i = 0; i < context->vertices; i++) {
            int edge_weight = context->graph_interface->get_edge(graph, node, i);
            if (edge_weight > 0) {
                if (i == start && (edge_weight > 1 || stack->len > 2)) {
                    char cycle_str[512];
                    normalize_cycle(stack, cycle_str);

                    if (!g_hash_table_contains(unique_cycles, cycle_str)) {
                        g_hash_table_add(unique_cycles, strdup(cycle_str));
                        
                        if (stack->len > max_cycle_length) {
                            max_cycle_length = stack->len;
                            g_array_set_size(output_cycles, 0);
                            cycle_count = 0;
                            store_cycle(stack);
                            cycle_count++;
                        } else if (stack->len == max_cycle_length) {
                            store_cycle(stack);
                            cycle_count++;
                        }
                    }
                } else if (!g_hash_table_contains(visited, GINT_TO_POINTER(i))) {
                    dfs(i, start, stack, visited);
                }
            }
        }

        g_array_remove_index(stack, stack->len - 1);
        g_hash_table_remove(visited, GINT_TO_POINTER(node));
    }

    for (int i = 0; i < context->vertices; i++) {
        GArray *stack = g_array_new(FALSE, FALSE, sizeof(int));
        GHashTable *visited = g_hash_table_new(g_direct_hash, g_direct_equal);
        dfs(i, i, stack, visited);
        g_array_free(stack, TRUE);
        g_hash_table_destroy(visited);
    }

    g_hash_table_destroy(unique_cycles);
    destroy_context(context);

    return cycle_count;
}

// int approximate_find_maximal_cycles(void *graph, int vertices, GArray *output_cycles) {
//     GraphAlgorithmContext *context = create_context(graph, vertices);
//     int approximate_max_cycle_length = 0;

//     // Random traversal to find maximal cycles
//     for (int i = 0; i < vertices; i++) {
//         GArray *current_cycle = g_array_new(FALSE, FALSE, sizeof(int));
//         int visited[vertices];
//         memset(visited, 0, sizeof(visited));
//         int node = i, length = 0;

//         while (length < vertices) {
//             visited[node] = 1;
//             g_array_append_val(current_cycle, node);
//             length++;

//             int found_next = 0;
//             for (int j = 0; j < vertices; j++) {
//                 if (!visited[j] && context->graph_interface->get_edge(graph, node, j) > 0) {
//                     node = j;
//                     found_next = 1;
//                     break;
//                 }
//             }

//             if (!found_next) break;
//         }

//         if (length > approximate_max_cycle_length) {
//             approximate_max_cycle_length = length;
//             g_array_set_size(output_cycles, 0); // Clear previous cycles
//             g_array_append_val(output_cycles, current_cycle);
//         } else {
//             g_array_free(current_cycle, TRUE);
//         }
//     }

//     destroy_context(context);
//     return approximate_max_cycle_length;
// }

GraphAlgorithm default_algorithm = {
    .calculate_size = calculate_size,
    .find_cycles = find_cycles,
    .count_hamiltonian_cycles = count_hamiltonian_cycles,
    .calculate_metric = calculate_metric,
    .find_minimal_extension = find_minimal_extension,
    .find_maximal_cycles = find_maximal_cycles,
    .count_maximal_cycles = count_maximal_cycles,

    // .approximate_find_cycles = approximate_find_cycles,
    // .approximate_count_hamiltonian_cycles = approximate_count_hamiltonian_cycles,
    // .approximate_calculate_metric = approximate_calculate_metric,
    // .approximate_find_minimal_extension = approximate_find_minimal_extension,
    // .approximate_find_maximal_cycles = approximate_find_maximal_cycles
};
