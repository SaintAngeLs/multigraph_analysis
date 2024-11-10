#ifndef UTILS_H
#define UTILS_H

#include "graph.h" 

int** allocate_matrix(int size);

void free_matrix(int **matrix, int size);

Graph* load_graph_from_file(const char *filename);


#endif
