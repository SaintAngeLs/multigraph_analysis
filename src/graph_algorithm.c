#include <stdio.h>
#include <glib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include "graph_algorithm.h"
#include "graph_interface.h"

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
    int *array = (int *)cycle->data;
    int len = cycle->len;
    int *sorted_array = malloc(len * sizeof(int));

    memcpy(sorted_array, array, len * sizeof(int));
    qsort(sorted_array, len, sizeof(int), int_cmp);

    int min_idx = 0;
    for (int i = 1; i < len; i++) {
        if (sorted_array[i] < sorted_array[min_idx]) {
            min_idx = i;
        }
    }

    int offset = 0;
    for (int i = 0; i < len; i++) {
        int idx = (min_idx + i) % len;
        offset += sprintf(buffer + offset, "%d-", sorted_array[idx]);
    }

    free(sorted_array);
}

static int find_cycles(void *graph, int vertices, GArray *output_cycles) {
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
                if (i == start && stack->len > 2) {
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


static int count_hamiltonian_cycles(void *graph, int vertices, GArray *output_cycles) {
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
            if (context->graph_interface->get_edge(graph, node, start) > 0) {
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


static GHashTable *calculate_degree_distribution(void* graph, int n) {
    GraphAlgorithmContext *context = create_context(graph, n);
    GHashTable *degree_count = g_hash_table_new(g_int_hash, g_int_equal);

    // Calculate degrees for each node
    for (int i = 0; i < n; i++) {
        int degree = 0;
        for (int j = 0; j < n; j++) {
            degree += context->graph_interface->get_edge(graph, i, j);
        }

        int *key = g_new(int, 1);
        *key = degree;
        int *value = g_hash_table_lookup(degree_count, key);
        if (value == NULL) {
            int *new_value = g_new(int, 1);
            *new_value = 1;
            g_hash_table_insert(degree_count, key, new_value);
        } else {
            (*value)++;
            g_free(key);
        }
    }

    // Normalize the degree distribution
    int total_nodes = n;
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, degree_count);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        double *normalized_value = g_new(double, 1);
        *normalized_value = *((int *)value) / (double)total_nodes;
        g_hash_table_insert(degree_count, key, normalized_value);
        g_free(value);
    }

    return degree_count;
}

static gint g_int_compare(gconstpointer a, gconstpointer b) {
    return *((int *)a) - *((int *)b);
}

static double calculate_emd(GHashTable *dist1, GHashTable *dist2) {
    // Combine keys from both distributions
    GHashTableIter iter;
    gpointer key, value;

    GList *all_degrees = NULL;
    g_hash_table_iter_init(&iter, dist1);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        all_degrees = g_list_append(all_degrees, key);
    }
    g_hash_table_iter_init(&iter, dist2);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        gboolean exists = FALSE;
        for (GList *node = all_degrees; node != NULL; node = node->next) {
            if (*((int *)node->data) == *((int *)key)) {
                exists = TRUE;
                break;
            }
        }
        if (!exists) {
            all_degrees = g_list_append(all_degrees, key);
        }
    }

    all_degrees = g_list_sort(all_degrees, (GCompareFunc)g_int_compare);

    double cumulative_dist1 = 0, cumulative_dist2 = 0;
    double emd = 0;

    for (GList *node = all_degrees; node != NULL; node = node->next) {
        int *degree = (int *)node->data;
        double prob1 = 0, prob2 = 0;

        if (g_hash_table_contains(dist1, degree)) {
            prob1 = *((double *)g_hash_table_lookup(dist1, degree));
        }
        if (g_hash_table_contains(dist2, degree)) {
            prob2 = *((double *)g_hash_table_lookup(dist2, degree));
        }

        cumulative_dist1 += prob1;
        cumulative_dist2 += prob2;

        emd += fabs(cumulative_dist1 - cumulative_dist2);
    }

    g_list_free(all_degrees);
    return emd;
}

static double calculate_metric(void *graph_1, int vertices_1, void *graph_2, int vertices_2) {
    GHashTable *dist1 = calculate_degree_distribution(graph_1, vertices_1);
    GHashTable *dist2 = calculate_degree_distribution(graph_2, vertices_2);

    double emd = calculate_emd(dist1, dist2);

    g_hash_table_destroy(dist1);
    g_hash_table_destroy(dist2);

    return emd;
}

static int find_minimal_extension(void *graph, int vertices) {
    GraphAlgorithmContext *context = create_context(graph, vertices);
    int min_edges_needed = INT_MAX;

    void dfs(int node, GHashTable *visited, int depth, int *lacking_edges) {
        g_hash_table_add(visited, GINT_TO_POINTER(node));

        if (depth == context->vertices) {
            if (context->graph_interface->get_edge(graph, node, 
                GPOINTER_TO_INT(g_hash_table_lookup(visited, GINT_TO_POINTER(0)))) == 0) {
                (*lacking_edges)++;
            }
            if (*lacking_edges < min_edges_needed) {
                min_edges_needed = *lacking_edges;
            }
        } else {
            for (int i = 0; i < context->vertices; i++) {
                if (context->graph_interface->get_edge(graph, node, i) > 0 &&
                    !g_hash_table_contains(visited, GINT_TO_POINTER(i))) {
                    dfs(i, visited, depth + 1, lacking_edges);
                }
            }
        }

        g_hash_table_remove(visited, GINT_TO_POINTER(node));
    }

    for (int i = 0; i < context->vertices; i++) {
        GHashTable *visited = g_hash_table_new(g_direct_hash, g_direct_equal);
        int lacking_edges = 0;
        dfs(i, visited, 1, &lacking_edges);
        g_hash_table_destroy(visited);
    }

    destroy_context(context);
    return min_edges_needed;
}

static int count_maximal_cycles(void *graph, int vertices) {
    GraphAlgorithmContext *context = create_context(graph, vertices);
    int max_cycle_length = 0;

    void dfs(int node, GHashTable *visited, int depth, int start) {
        g_hash_table_add(visited, GINT_TO_POINTER(node));

        for (int i = 0; i < context->vertices; i++) {
            if (context->graph_interface->get_edge(graph, node, i) > 0) {
                if (i == start && depth > max_cycle_length) {
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

static int find_maximal_cycles(void *graph, int vertices, GArray *output_cycles) {
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
                if (i == start && stack->len > 2) {
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


GraphAlgorithm default_algorithm = {
    .calculate_size = calculate_size,
    .find_cycles = find_cycles,
    .count_hamiltonian_cycles = count_hamiltonian_cycles,
    .calculate_metric = calculate_metric,
    .find_minimal_extension = find_minimal_extension,
    .find_maximal_cycles = find_maximal_cycles,
    .count_maximal_cycles = count_maximal_cycles,
};
