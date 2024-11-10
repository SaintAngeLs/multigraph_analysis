#include <stdlib.h>
#include "graph.h"
#include "utils.h"

Graph* create_graph(int vertices) {
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    graph->vertices = vertices;
    graph->adjacency_matrix = allocate_matrix(vertices);
    return graph;
}

void add_edge(Graph *graph, int src, int dest, int weight) {
    if (src < graph->vertices && dest < graph->vertices) {
        graph->adjacency_matrix[src][dest] = weight;
    }
}

void free_graph(Graph *graph) {
    free_matrix(graph->adjacency_matrix, graph->vertices);
    free(graph);
}
