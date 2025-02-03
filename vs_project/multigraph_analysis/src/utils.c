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

void init_graph_array(GraphArray* arr, size_t capacity) {
    arr->data = (GraphInterface**)malloc(capacity * sizeof(GraphInterface*));
    arr->size = 0;
    arr->capacity = capacity;
}
void add_graph(GraphArray* arr, GraphInterface* graph) {
    if (arr->size >= arr->capacity) {
        arr->capacity *= 2;
        arr->data = (GraphInterface**)realloc(arr->data, arr->capacity * sizeof(GraphInterface*));
    }
    arr->data[arr->size++] = graph;
}
void free_graph_array(GraphArray* arr) {
    free(arr->data);
}

void print_cycles(GraphInterface* multigraph, int** output_cycles, int* cycle_sizes, int cycle_count) {
    for (int i = 0; i < cycle_count; i++) {
        for (int j = 0; j < cycle_sizes[i]; j++) {
            printf("%d", output_cycles[i][j]);
            if (j < cycle_sizes[i] - 1) {
                printf(" -> ");
            }
        }
        printf("\n");
        /* Free each cycle array after printing. */
        free(output_cycles[i]);
    }
    free(output_cycles);
    free(cycle_sizes);
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
    Multigraph* multigraph_handle;
    GraphInterface *multigraph_interface = create_multigraph(vertices, &multigraph_handle);
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
    free_multigraph(multigraph_handle);

    fprintf(stdout, "Adjacency matrix successfully read.\n");

    fclose(file);
    fprintf(stdout, "File closed successfully.\n");
    return multigraph_interface;
}
