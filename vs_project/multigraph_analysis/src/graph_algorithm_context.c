#include "graph_algorithm_context.h"

GraphAlgorithmContext* create_context(void* graph, int vertices) {
    GraphAlgorithmContext* ctx = (GraphAlgorithmContext*)malloc(sizeof(GraphAlgorithmContext));
    if (!ctx) {
        fprintf(stderr, "Error: failed to allocate GraphAlgorithmContext\n");
        return NULL;
    }
    ctx->graph_interface = (GraphInterface*)graph;
    ctx->vertices = vertices;
    return ctx;
}

void destroy_context(GraphAlgorithmContext* context) {
    if (context) free(context);
}