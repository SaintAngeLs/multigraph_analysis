#ifndef GRAPH_ALGORITHM_CONTEXT_H
#define GRAPH_ALGORITHM_CONTEXT_H

#include "graph_interface.h"

typedef struct {
    GraphInterface* graph_interface;
    int vertices;
} GraphAlgorithmContext;

GraphAlgorithmContext* create_context(void* graph, int vertices);

void destroy_context(GraphAlgorithmContext* context);

#endif // GRAPH_ALGORITHM_CONTEXT_H