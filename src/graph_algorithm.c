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

static GArray* calculate_metric(void *graph, int vertices) {
    GraphAlgorithmContext *context = create_context(graph, vertices);
    int max_edge = 0;

    for (int i = 0; i < context->vertices; i++) {
        for (int j = 0; j < context->vertices; j++) {
            int edge_weight = context->graph_interface->get_edge(graph, i, j);
            if (edge_weight > max_edge) {
                max_edge = edge_weight;
            }
        }
    }

    GArray *metric = g_array_new(FALSE, FALSE, sizeof(int));
    for (int k = 0; k <= max_edge; k++) {
        int count = 0;
        g_array_append_val(metric, count);
    }

    for (int i = 0; i < context->vertices; i++) {
        for (int j = i + 1; j < context->vertices; j++) {
            int edge_count = context->graph_interface->get_edge(graph, i, j);
            if (edge_count > 0) {
                int current_value = g_array_index(metric, int, edge_count);
                g_array_index(metric, int, edge_count) = current_value + 1;
            }
        }
    }

    destroy_context(context);
    return metric;
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

GraphAlgorithm default_algorithm = {
    .calculate_size = calculate_size,
    .find_cycles = find_cycles,
    .count_hamiltonian_cycles = count_hamiltonian_cycles,
    .calculate_metric = calculate_metric,
    .find_minimal_extension = find_minimal_extension,
};
