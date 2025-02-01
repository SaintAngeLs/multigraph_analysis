#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include "graph_interface.h"


typedef struct GraphTag {
    int vertices;
    int** adjacency_matrix;
} Graph;

typedef struct MultigraphTag {
    GraphInterface graph_interface;
    int vertices;
    int** adjacency_matrix;
} Multigraph;


Graph* create_graph(int vertices);

void add_edge(Graph *graph, int src, int dest, int weight);

int get_edge(const Graph *graph, int src, int dest);

int calculate_size(const Graph *graph);

void destroy_graph(Graph *graph);


GraphInterface* create_multigraph(int vertices);

#endif // GRAPH_H
