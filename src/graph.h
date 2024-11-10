#ifndef GRAPH_H
#define GRAPH_H

typedef struct {
    int vertices;
    int **adjacency_matrix;
} Graph;

Graph* create_graph(int vertices);

void add_edge(Graph *graph, int src, int dest, int weight);

void free_graph(Graph *graph);

#endif
