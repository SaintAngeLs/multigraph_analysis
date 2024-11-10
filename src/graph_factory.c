#include <stdlib.h>
#include "graph.h"
#include "graph_factory.h"
#include "utils.h"

Graph* create_graph_with_type(GraphType type, int vertices) {
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    if (!graph) return NULL;

    graph->vertices = vertices;
    graph->adjacency_matrix = allocate_matrix(vertices);

    if (type == MULTIGRAPH) {
        //  ... add support for multigraphs
    }

    return graph;
}
