#include "utils.h"

int** allocate_matrix(int vertices) {
    int **matrix = (int **)malloc(vertices * sizeof(int *));
    if (!matrix) {
        fprintf(stderr, "Failed to allocate matrix rows.\n");
        return NULL;
    }

    for (int i = 0; i < vertices; i++) {
        matrix[i] = (int *)calloc(vertices, sizeof(int));
        if (!matrix[i]) {
            fprintf(stderr, "Failed to allocate matrix row %d.\n", i);
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            return NULL;
        }
    }
    return matrix;
}

void free_matrix(int **matrix, int size) {
    for (int i = 0; i < size; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

GraphInterface* load_multigraph_from_file(const char *filename) {
    fprintf(stdout, "Opening file: %s\n", filename);

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    int num_graphs;
    fprintf(stdout, "Reading number of graphs from file...\n");
    if (fscanf(file, "%d", &num_graphs) != 1 || num_graphs <= 0) {
        fprintf(stderr, "Invalid number of graphs in file. Expected a positive integer.\n");
        fclose(file);
        return NULL;
    }
    fprintf(stdout, "Number of graphs: %d\n", num_graphs);

    int vertices;
    fprintf(stdout, "Reading number of vertices for the first graph...\n");
    if (fscanf(file, "%d", &vertices) != 1 || vertices <= 0) {
        fprintf(stderr, "Invalid vertex count in file. Expected a positive integer.\n");
        fclose(file);
        return NULL;
    }
    fprintf(stdout, "Number of vertices: %d\n", vertices);

    fprintf(stdout, "Creating multigraph with %d vertices...\n", vertices);
    GraphInterface *multigraph_interface = create_multigraph(vertices);
    if (!multigraph_interface) {
        fprintf(stderr, "Failed to create multigraph structure.\n");
        fclose(file);
        return NULL;
    }
    fprintf(stdout, "Multigraph created successfully.\n");

    fprintf(stdout, "Reading adjacency matrix...\n");
    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            int edge_count;
            if (fscanf(file, "%d", &edge_count) != 1) {
                fprintf(stderr, "Invalid adjacency matrix data at row %d, column %d.\n", i, j);
                multigraph_interface->destroy(multigraph_interface);
                fclose(file);
                return NULL;
            }

            if (edge_count < 0) {
                fprintf(stderr, "Invalid edge weight (%d) at row %d, column %d.\n", edge_count, i, j);
                multigraph_interface->destroy(multigraph_interface);
                fclose(file);
                return NULL;
            }

            fprintf(stdout, "Edge from %d to %d with weight %d.\n", i, j, edge_count);
            if (edge_count > 0) {
                multigraph_interface->add_edge(multigraph_interface, i, j, edge_count);
            }
        }
    }
    fprintf(stdout, "Adjacency matrix successfully read.\n");

    fclose(file);
    fprintf(stdout, "File closed successfully.\n");
    return multigraph_interface;
}



