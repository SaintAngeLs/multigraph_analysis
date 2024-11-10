#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "graph.h"

int** allocate_matrix(int size) {
    int **matrix = (int **)malloc(size * sizeof(int *));
    for (int i = 0; i < size; i++) {
        matrix[i] = (int *)calloc(size, sizeof(int));
    }
    return matrix;
}

void free_matrix(int **matrix, int size) {
    for (int i = 0; i < size; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

Graph* load_graph_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return NULL;

    int vertices;
    fscanf(file, "%d", &vertices);
    Graph *graph = create_graph(vertices);

    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            fscanf(file, "%d", &graph->adjacency_matrix[i][j]);
        }
    }

    fclose(file);
    return graph;
}
