#include <stdio.h>
#include <stdlib.h>
#include "glib.h"
#include "graph.h"
#include "graph_factory.h"
#include "graph_interface.h"
#include "utils.h"


GraphInterface* create_graph_with_type(GraphType type, int vertices) {
    if (type == SIMPLE_GRAPH) {
        Graph *simple_graph = create_graph(vertices);
        
        if (!simple_graph) {
            return NULL;
        }
        return (GraphInterface*)simple_graph;
    } else if (type == MULTIGRAPH) {
        return create_multigraph(vertices);
    }

    fprintf(stderr, "Invalid graph type specified.\n");
    return NULL;
}
