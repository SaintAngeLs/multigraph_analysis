#ifndef GRAPH_H
#define GRAPH_H

#include "graph_interface.h"


typedef struct {

    int vertices;

    int **adjacency_matrix;

} Graph;


typedef struct {

    GraphInterface interface;

    int vertices;

    int **adjacency_matrix;

} Multigraph;


Graph* create_graph(int vertices);

void add_edge(Graph *graph, int src, int dest, int weight);

int get_edge(const Graph *graph, int src, int dest);

int calculate_size(const Graph *graph);

void destroy_graph(Graph *graph);


GraphInterface* create_multigraph(int vertices);

#endif
