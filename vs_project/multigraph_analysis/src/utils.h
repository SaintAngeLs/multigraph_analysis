#ifndef UTILS_H
#define UTILS_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "graph.h" 

typedef struct {
    GraphInterface** data;
    size_t size;
    size_t capacity;
} GraphArray;

int** allocate_matrix(int size);

void free_matrix(int **matrix, int size);

// Graph* load_graph_from_file(const char *filename);

GraphInterface* load_multigraph_from_file(const char *filename);

#endif // UTILS_H
