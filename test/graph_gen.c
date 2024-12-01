#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Function to generate a complete graph
void generate_complete_graph(int vertices, int max_weight, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%d\n", vertices);
    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            if (i != j) {
                fprintf(file, "%d ", rand() % max_weight + 1);
            } else {
                fprintf(file, "0 ");
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

// Function to generate a sparse graph
void generate_sparse_graph(int vertices, double edge_probability, int max_weight, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%d\n", vertices);
    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            if (i != j && (rand() / (double)RAND_MAX) < edge_probability) {
                fprintf(file, "%d ", rand() % max_weight + 1);
            } else {
                fprintf(file, "0 ");
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

// Function to generate a bipartite graph
void generate_bipartite_graph(int set_size, int max_weight, const char *filename) {
    int total_vertices = 2 * set_size;
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%d\n", total_vertices);
    for (int i = 0; i < total_vertices; i++) {
        for (int j = 0; j < total_vertices; j++) {
            if (i < set_size && j >= set_size) {
                fprintf(file, "%d ", rand() % max_weight + 1);
            } else if (i >= set_size && j < set_size) {
                fprintf(file, "%d ", rand() % max_weight + 1);
            } else {
                fprintf(file, "0 ");
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

// Main program
int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Usage: %s <graph_type> <vertices> <max_weight> <output_file> [edge_probability]\n", argv[0]);
        fprintf(stderr, "graph_type: complete | sparse | bipartite\n");
        fprintf(stderr, "vertices: Number of vertices (for bipartite, this is total vertices in both sets)\n");
        fprintf(stderr, "max_weight: Maximum weight of an edge\n");
        fprintf(stderr, "output_file: File to save the graph\n");
        fprintf(stderr, "edge_probability: Optional, for sparse graphs only (e.g., 0.2)\n");
        return EXIT_FAILURE;
    }

    const char *graph_type = argv[1];
    int vertices = atoi(argv[2]);
    int max_weight = atoi(argv[3]);
    const char *output_file = argv[4];
    double edge_probability = 0.0;

    if (strcmp(graph_type, "sparse") == 0 && argc >= 6) {
        edge_probability = atof(argv[5]);
    }

    srand(time(NULL)); // Initialize random seed

    if (strcmp(graph_type, "complete") == 0) {
        generate_complete_graph(vertices, max_weight, output_file);
    } else if (strcmp(graph_type, "sparse") == 0) {
        if (edge_probability <= 0.0 || edge_probability > 1.0) {
            fprintf(stderr, "Error: For sparse graphs, edge_probability must be between 0.0 and 1.0\n");
            return EXIT_FAILURE;
        }
        generate_sparse_graph(vertices, edge_probability, max_weight, output_file);
    } else if (strcmp(graph_type, "bipartite") == 0) {
        if (vertices % 2 != 0) {
            fprintf(stderr, "Error: For bipartite graphs, vertices must be even\n");
            return EXIT_FAILURE;
        }
        generate_bipartite_graph(vertices / 2, max_weight, output_file);
    } else {
        fprintf(stderr, "Error: Invalid graph type. Choose from 'complete', 'sparse', or 'bipartite'.\n");
        return EXIT_FAILURE;
    }

    printf("Graph saved to %s\n", output_file);
    return EXIT_SUCCESS;
}
