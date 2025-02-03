#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "graph.h" 

int** allocate_matrix(int size);

void free_matrix(int **matrix, int size);

// Graph* load_graph_from_file(const char *filename);

GraphInterface* load_multigraph_from_file(const char *filename);

#endif // UTILS_H
