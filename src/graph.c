#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "graph_interface.h"
#include "utils.h"


Graph* create_graph(int vertices) {
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    if (!graph) {
        fprintf(stderr, "Failed to allocate memory for graph.\n");
        return NULL;
    }

    graph->vertices = vertices;
    graph->adjacency_matrix = allocate_matrix(vertices);
    if (!graph->adjacency_matrix) {
        fprintf(stderr, "Failed to allocate adjacency matrix.\n");
        free(graph);
        return NULL;
    }

    return graph;
}

void add_edge(Graph *graph, int src, int dest, int weight) {
    if (src < graph->vertices && dest < graph->vertices) {
        graph->adjacency_matrix[src][dest] = weight; // Simple graph: only set weight once
    }
}

int get_edge(const Graph *graph, int src, int dest) {
    if (src < graph->vertices && dest < graph->vertices) {
        return graph->adjacency_matrix[src][dest];
    }
    return 0;
}

int calculate_size(const Graph *graph) {
    int size = 0;
    for (int i = 0; i < graph->vertices; i++) {
        for (int j = i; j < graph->vertices; j++) {
            size += graph->adjacency_matrix[i][j];
        }
    }
    return size;
}

void destroy_graph(Graph *graph) {
    free_matrix(graph->adjacency_matrix, graph->vertices);
    free(graph);
}

// Multigraph-Specific Functions
static void multigraph_add_edge(void *self, int src, int dest, int weight) {
    Multigraph *graph = (Multigraph *)self;
    if (src < graph->vertices && dest < graph->vertices) {
        graph->adjacency_matrix[src][dest] += weight; // Multigraph: accumulate weights
    }
}

static int multigraph_get_edge(void *self, int src, int dest) {
    Multigraph *graph = (Multigraph *)self;
    if (src < graph->vertices && dest < graph->vertices) {
        return graph->adjacency_matrix[src][dest];
    }
    return 0;
}

static int multigraph_calculate_size(void *self) {
    Multigraph *graph = (Multigraph *)self;
    int size = 0;
    for (int i = 0; i < graph->vertices; i++) {
        for (int j = i; j < graph->vertices; j++) {
            size += graph->adjacency_matrix[i][j];
        }
    }
    return size;
}

static void multigraph_destroy(void *self) {
    Multigraph *graph = (Multigraph *)self;
    free_matrix(graph->adjacency_matrix, graph->vertices);
    free(graph);
}

GraphInterface* create_multigraph(int vertices) {
    Multigraph *graph = (Multigraph *)malloc(sizeof(Multigraph));
    if (!graph) {
        fprintf(stderr, "Failed to allocate memory for multigraph.\n");
        return NULL;
    }

    graph->vertices = vertices;
    graph->adjacency_matrix = allocate_matrix(vertices);
    if (!graph->adjacency_matrix) {
        fprintf(stderr, "Failed to allocate adjacency matrix.\n");
        free(graph);
        return NULL;
    }

    graph->interface.add_edge = multigraph_add_edge;
    graph->interface.get_edge = multigraph_get_edge;
    graph->interface.calculate_size = multigraph_calculate_size;
    graph->interface.destroy = multigraph_destroy;
    graph->interface.vertices = vertices; 

    return &graph->interface;
}
