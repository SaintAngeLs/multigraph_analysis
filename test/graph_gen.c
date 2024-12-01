#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Function to generate a complete multigraph
void generate_complete_multigraph(int vertices, int max_weight, int max_edges, FILE *file) {
    fprintf(file, "%d\n", vertices); // Number of vertices
    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            if (i != j) {
                int edges = rand() % max_edges + 1; // Number of edges between i and j
                fprintf(file, "%d ", edges); // Write the number of edges
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

// Function to generate a complete graph
void generate_complete_graph(int vertices, int max_weight, FILE *file) {
    fprintf(file, "%d\n", vertices); // Number of vertices
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
}

// Function to generate a sparse graph
void generate_sparse_graph(int vertices, double edge_probability, int max_weight, FILE *file) {
    fprintf(file, "%d\n", vertices); // Number of vertices
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
}

// Function to generate a bipartite graph
void generate_bipartite_graph(int set_size, int max_weight, FILE *file) {
    int total_vertices = 2 * set_size;
    fprintf(file, "%d\n", total_vertices); // Number of vertices
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
}

// Main program
int main(int argc, char *argv[]) {
    if (argc < 6) {
        fprintf(stderr, "Usage: %s <graph_type> <num_graphs> <vertices> <max_weight> <output_file> [extra_param]\n", argv[0]);
        fprintf(stderr, "graph_type: complete | sparse | bipartite | multigraph\n");
        fprintf(stderr, "num_graphs: Number of graphs to generate\n");
        fprintf(stderr, "vertices: Number of vertices (for bipartite, this is total vertices in both sets)\n");
        fprintf(stderr, "max_weight: Maximum weight of an edge\n");
        fprintf(stderr, "output_file: File to save the graphs\n");
        fprintf(stderr, "extra_param: For sparse, edge_probability (e.g., 0.2). For multigraph, max_edges.\n");
        return EXIT_FAILURE;
    }

    const char *graph_type = argv[1];
    int num_graphs = atoi(argv[2]);
    int vertices = atoi(argv[3]);
    int max_weight = atoi(argv[4]);
    const char *output_file = argv[5];
    double edge_probability = 0.0;
    int max_edges = 0;

    if (strcmp(graph_type, "sparse") == 0 && argc >= 7) {
        edge_probability = atof(argv[6]);
    } else if (strcmp(graph_type, "multigraph") == 0 && argc >= 7) {
        max_edges = atoi(argv[6]);
        if (max_edges <= 0) {
            fprintf(stderr, "Error: max_edges must be a positive integer for multigraphs\n");
            return EXIT_FAILURE;
        }
    }

    srand(time(NULL)); // Initialize random seed

    FILE *file = fopen(output_file, "w");
    if (!file) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    fprintf(file, "%d\n", num_graphs); // Number of graphs
    for (int g = 0; g < num_graphs; g++) {
        if (strcmp(graph_type, "complete") == 0) {
            generate_complete_graph(vertices, max_weight, file);
        } else if (strcmp(graph_type, "sparse") == 0) {
            if (edge_probability <= 0.0 || edge_probability > 1.0) {
                fprintf(stderr, "Error: For sparse graphs, edge_probability must be between 0.0 and 1.0\n");
                fclose(file);
                return EXIT_FAILURE;
            }
            generate_sparse_graph(vertices, edge_probability, max_weight, file);
        } else if (strcmp(graph_type, "bipartite") == 0) {
            if (vertices % 2 != 0) {
                fprintf(stderr, "Error: For bipartite graphs, vertices must be even\n");
                fclose(file);
                return EXIT_FAILURE;
            }
            generate_bipartite_graph(vertices / 2, max_weight, file);
        } else if (strcmp(graph_type, "multigraph") == 0) {
            generate_complete_multigraph(vertices, max_weight, max_edges, file);
        } else {
            fprintf(stderr, "Error: Invalid graph type. Choose from 'complete', 'sparse', 'bipartite', or 'multigraph'.\n");
            fclose(file);
            return EXIT_FAILURE;
        }
    }

    fclose(file);
    printf("Graphs saved to %s\n", output_file);
    return EXIT_SUCCESS;
}
