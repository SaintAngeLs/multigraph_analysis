#include <stdio.h>
#include <glib.h>
#include <stdbool.h>
#include <limits.h>
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


static int find_cycles(void *graph, int vertices) {
    GraphAlgorithmContext *context = create_context(graph, vertices);
    GArray *stack = g_array_new(FALSE, FALSE, sizeof(int));
    GHashTable *visited = g_hash_table_new(g_direct_hash, g_direct_equal);
    int cycle_count = 0;

    void dfs(int node, int start) {
        g_array_append_val(stack, node);
        g_hash_table_add(visited, GINT_TO_POINTER(node));

        for (int i = 0; i < context->vertices; i++) {
            if (context->graph_interface->get_edge(graph, node, i) > 0) {
                if (i == start) {
                    cycle_count++;
                } else if (!g_hash_table_contains(visited, GINT_TO_POINTER(i))) {
                    dfs(i, start);
                }
            }
        }

        g_array_remove_index(stack, stack->len - 1);
        g_hash_table_remove(visited, GINT_TO_POINTER(node));
    }

    for (int i = 0; i < context->vertices; i++) {
        dfs(i, i);
    }

    g_array_free(stack, TRUE);
    g_hash_table_destroy(visited);
    destroy_context(context);

    return cycle_count / 2;
}

static int count_hamiltonian_cycles(void *graph, int vertices) {
    GraphAlgorithmContext *context = create_context(graph, vertices);
    int count = 0;
    GArray *path = g_array_new(FALSE, FALSE, sizeof(int));
    GHashTable *visited = g_hash_table_new(g_direct_hash, g_direct_equal);

    void backtrack(int node, int start, int depth) {
        g_array_append_val(path, node);
        g_hash_table_add(visited, GINT_TO_POINTER(node));

        if (depth == context->vertices) {
            if (context->graph_interface->get_edge(graph, node, start) > 0) {
                count++;
            }
        } else {
            for (int i = 0; i < context->vertices; i++) {
                if (!g_hash_table_contains(visited, GINT_TO_POINTER(i)) &&
                    context->graph_interface->get_edge(graph, node, i) > 0) {
                    backtrack(i, start, depth + 1);
                }
            }
        }

        g_array_remove_index(path, path->len - 1);
        g_hash_table_remove(visited, GINT_TO_POINTER(node));
    }

    for (int i = 0; i < context->vertices; i++) {
        backtrack(i, i, 1);
    }

    g_array_free(path, TRUE);
    g_hash_table_destroy(visited);
    destroy_context(context);

    return count;
}

static bool are_arrays_equal(GArray *array1, GArray *array2) {
    if (array1->len != array2->len) {
        return false;
    }

    for (guint i = 0; i < array1->len; i++) {
        int value1 = g_array_index(array1, int, i);
        int value2 = g_array_index(array2, int, i);
        if (value1 != value2) {
            return false;
        }
    }

    return true;
}

static GArray* calculate_metric(void* graph, int vertices) {
    GraphAlgorithmContext *context = create_context(graph, vertices);
    int distance_matrix[vertices][vertices];
    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            if (i == j) {
                distance_matrix[i][j] = 0;
            } else {
                int edge_weight = context->graph_interface->get_edge(graph, i, j);
                distance_matrix[i][j] = (edge_weight > 0) ? 1 : INT_MAX;
            }
        }
    }

    // Change to better algorithm
    for (int k = 0; k < vertices; k++) {
        for (int i = 0; i < vertices; i++) {
            for (int j = 0; j < vertices; j++) {
                if (distance_matrix[i][k] < INT_MAX && distance_matrix[k][j] < INT_MAX) {
                    int new_distance = distance_matrix[i][k] + distance_matrix[k][j];
                    if (new_distance < distance_matrix[i][j]) {
                        distance_matrix[i][j] = new_distance;
                    }
                }
            }
        }
    }

    bool is_resolving_set(GArray* subset) {
        if (subset->len == 0) {
            return FALSE;
        }

        GArray *signatures = g_array_new(FALSE, FALSE, sizeof(GArray *));

        for (int v = 0; v < vertices; v++) {
            GArray* signature = g_array_new(FALSE, FALSE, sizeof(int));
            for (int i = 0; i < subset->len; i++) {
                int s = g_array_index(subset, int, i);
                g_array_append_val(signature, distance_matrix[v][s]);
            }

            gboolean existing = FALSE;
            for(int i = 0; i < signatures->len; i++) {
                GArray *existing_array = g_array_index(signatures, GArray *, i);
                if (are_arrays_equal(existing_array, signature)) {
                    existing = TRUE;
                    break;
                }
            }
            if (existing) {
                for (int i = 0; i < signatures->len; i++) {
                    GArray *inner_array = g_array_index(signatures, GArray *, i);
                    g_array_free(inner_array, TRUE);
                }
                g_array_free(signatures, TRUE);
                g_array_free(signature, TRUE);
                return FALSE;
            }
            g_array_append_val(signatures, signature);
        }

        for (int i = 0; i < signatures->len; i++) {
            GArray *inner_array = g_array_index(signatures, GArray *, i);
            g_array_free(inner_array, TRUE);
        }
        g_array_free(signatures, TRUE);
        return TRUE;
    }

    // Find the smallest resolving set
    GArray* smallest_resolving_set = g_array_new(FALSE, FALSE, sizeof(int));
    int totalSubsets = 1 << vertices;
    GArray* subset = g_array_new(FALSE, FALSE, sizeof(int));
    for (int it_i = 0; it_i < totalSubsets; it_i++) {
        for (int it_j = 0; it_j < vertices; it_j++) {
            if ((it_i >> it_j) & 1) {
                g_array_append_val(subset, it_j);
            }
        }
        if (is_resolving_set(subset)) {
            smallest_resolving_set = g_array_copy(subset);
            g_array_free(subset, TRUE);
            break;
        }
        g_array_remove_range(subset, 0, subset->len);
    }

    return smallest_resolving_set;
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

static int find_maximal_cycles(void *graph, int vertices) {
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

static int count_maximal_cycles(void *graph, int vertices) {
    GraphAlgorithmContext *context = create_context(graph, vertices);
    int max_cycle_count = 0;

    void dfs(int node, GHashTable *visited, int depth, int start) {
        g_hash_table_add(visited, GINT_TO_POINTER(node));

        for (int i = 0; i < context->vertices; i++) {
            if (context->graph_interface->get_edge(graph, node, i) > 0) {
                if (i == start && depth == context->vertices) {
                    max_cycle_count++;
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
    return max_cycle_count;
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
