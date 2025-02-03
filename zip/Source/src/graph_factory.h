#ifndef GRAPH_FACTORY_H
#define GRAPH_FACTORY_H

#include "graph.h"

typedef enum { SIMPLE_GRAPH, MULTIGRAPH } GraphType;

// void* create_graph_with_type(GraphType type, int vertices);

GraphInterface* create_graph_with_type(GraphType type, int vertices);

#endif
