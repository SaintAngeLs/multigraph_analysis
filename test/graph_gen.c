#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void generate_complete_multigraph(int vertices, int max_weight, int max_edges, FILE *file) {
    fprintf(file, "%d\n", vertices);
    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            if (i != j) {
                int edges = rand() % max_edges + 1; // Number of edges
                fprintf(file, "%d ", edges); // Write number of edges
                for (int e = 0; e < edges; e++) {
                    fprintf(file, "%d ", rand() % max_weight + 1); // Edge weights
                }
            } else {
                fprintf(file, "0 "); // No self-loops
            }
        }
        fprintf(file, "\n");
    }
}

void generate_sparse_multigraph(int vertices, double edge_probability, int max_weight, int max_edges, FILE *file) {
    // printf("Debug: Called generate_sparse_multigraph with arguments:\n");
    // printf("  vertices = %d\n", vertices);
    // printf("  edge_probability = %f\n", edge_probability);
    // printf("  max_weight = %d\n", max_weight);
    // printf("  max_edges = %d\n", max_edges);
    // printf("  file pointer = %p\n", (void *)file);

    fprintf(file, "%d\n", vertices); 
    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            double random_value = (double)rand() / (double)RAND_MAX; 
            if (i != j && random_value < edge_probability) {
                int edges = rand() % max_edges + 1;
                fprintf(file, "%d ", edges);
                for (int e = 0; e < edges; e++) {
                    fprintf(file, "%d ", rand() % max_weight + 1);
                }
            } else {
                fprintf(file, "0 ");
            }
        }
        fprintf(file, "\n");
    }
}

void generate_bipartite_multigraph(int set_size, int max_weight, int max_edges, FILE *file) {
    int total_vertices = 2 * set_size;
    fprintf(file, "%d\n", total_vertices);
    for (int i = 0; i < total_vertices; i++) {
        for (int j = 0; j < total_vertices; j++) {
            if (i < set_size && j >= set_size) {
                int edges = rand() % max_edges + 1;
                fprintf(file, "%d ", edges);
                for (int e = 0; e < edges; e++) {
                    fprintf(file, "%d ", rand() % max_weight + 1);
                }
            } else if (i >= set_size && j < set_size) {
                int edges = rand() % max_edges + 1;
                fprintf(file, "%d ", edges);
                for (int e = 0; e < edges; e++) {
                    fprintf(file, "%d ", rand() % max_weight + 1);
                }
            } else {
                fprintf(file, "0 ");
            }
        }
        fprintf(file, "\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 7) {
        fprintf(stderr, "Usage: %s <graph_type> <num_graphs> <vertices> <max_weight> <output_file> <max_edges/edge_probability>\n", argv[0]);
        fprintf(stderr, "graph_type: complete | sparse | bipartite\n");
        fprintf(stderr, "num_graphs: Number of graphs to generate\n");
        fprintf(stderr, "vertices: Number of vertices (for bipartite, total vertices in both sets)\n");
        fprintf(stderr, "max_weight: Maximum weight of an edge\n");
        fprintf(stderr, "output_file: File to save the graphs\n");
        fprintf(stderr, "max_edges: For complete and bipartite graphs, maximum edges between vertex pairs\n");
        fprintf(stderr, "edge_probability: For sparse graphs, probability of an edge existing\n");
        return EXIT_FAILURE;
    }

    const char *graph_type = argv[1];
    int num_graphs = atoi(argv[2]);
    int vertices = atoi(argv[3]);
    int max_weight = atoi(argv[4]);
    const char *output_file = argv[5];
    double edge_probability = 0.0;
    int max_edges = 0;

    // Parse the extra parameter based on graph type
    if (strcmp(graph_type, "sparse") == 0) {
        edge_probability = atof(argv[6]);
        if (edge_probability <= 0.0 || edge_probability > 1.0) {
            fprintf(stderr, "Error: For sparse graphs, edge_probability must be between 0.0 and 1.0\n");
            return EXIT_FAILURE;
        }
        // Default max_edges for sparse graphs if not specified
        max_edges = (argc > 7) ? atoi(argv[7]) : 5;
    } else {
        max_edges = atoi(argv[6]);
        if (max_edges <= 0) {
            fprintf(stderr, "Error: max_edges must be a positive integer\n");
            return EXIT_FAILURE;
        }
    }

    srand(time(NULL));

    FILE *file = fopen(output_file, "w");
    if (!file) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    fprintf(file, "%d\n", num_graphs);
    for (int g = 0; g < num_graphs; g++) {
        if (strcmp(graph_type, "complete") == 0) {
            generate_complete_multigraph(vertices, max_weight, max_edges, file);
        } else if (strcmp(graph_type, "sparse") == 0) {
            generate_sparse_multigraph(vertices, edge_probability, max_weight, max_edges, file);
        } else if (strcmp(graph_type, "bipartite") == 0) {
            if (vertices % 2 != 0) {
                fprintf(stderr, "Error: For bipartite graphs, vertices must be even\n");
                fclose(file);
                return EXIT_FAILURE;
            }
            generate_bipartite_multigraph(vertices / 2, max_weight, max_edges, file);
        } else {
            fprintf(stderr, "Error: Invalid graph type. Choose from 'complete', 'sparse', or 'bipartite'.\n");
            fclose(file);
            return EXIT_FAILURE;
        }
    }

    fclose(file);
    printf("Graphs saved to %s\n", output_file);
    return EXIT_SUCCESS;
}
