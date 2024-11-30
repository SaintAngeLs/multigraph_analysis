#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "graph.h"
#include "utils.h"
#include "config.h"
#include "graph_algorithm.h"  // Ensure this header is included

void handle_arguments(int argc, char *argv[], Config *config) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    config->input_file = argv[1];
}

FILE* open_file_with_retry(const char *filename) {
    FILE *file;
    do {
        file = fopen(filename, "r");
    } while (!file && errno == EINTR);

    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    return file;
}

void analyze_multigraph(GraphInterface *multigraph) {
    printf("Multigraph size: %d\n", multigraph->calculate_size(multigraph));
    printf("Cycle count: %d\n", default_algorithm.find_cycles(multigraph, multigraph->vertices));
    printf("Hamiltonian cycles: %d\n", default_algorithm.count_hamiltonian_cycles(multigraph, multigraph->vertices));

    GArray *metric = default_algorithm.calculate_metric(multigraph, multigraph->vertices);
    printf("Metric values:\n");
    for (int i = 0; i < metric->len; i++) {
        printf("Metric[%d] = %d\n", i, g_array_index(metric, int, i));
    }
    g_array_free(metric, TRUE);

    int minimalExtension = default_algorithm.find_minimal_extension(multigraph, multigraph->vertices);
    printf("Minimal extension for Hamiltonian cycle: %d\n", minimalExtension);
}

void cleanup_resources(GraphInterface *multigraph) {
    if (multigraph) {
        multigraph->destroy(multigraph);
    }
}

int main(int argc, char *argv[]) {
    Config *config = get_config(); // Ensure this retrieves a struct with config settings
    handle_arguments(argc, argv, config);

    FILE *file = open_file_with_retry(config->input_file);

    int num_graphs;
    fscanf(file, "%d", &num_graphs); // Read the number of graphs
    printf("Processing %d graphs.\n", num_graphs);

    for (int i = 0; i < num_graphs; i++) {
        int vertices;
        fscanf(file, "%d", &vertices); // Read the number of vertices for each graph

        GraphInterface *multigraph = create_multigraph(vertices);
        if (!multigraph) {
            fprintf(stderr, "Failed to initialize the graph interface.\n");
            continue;
        }

        // Load edges
        int src, dest, weight;
        for (int j = 0; j < vertices; j++) {
            for (int k = 0; k < vertices; k++) {
                fscanf(file, "%d", &weight);
                if (weight > 0) { // Only add actual edges
                    multigraph->add_edge(multigraph, j, k, weight);
                }
            }
        }

        // Analyze the graph
        analyze_multigraph(multigraph);
        cleanup_resources(multigraph);
    }

    fclose(file);
    return EXIT_SUCCESS;
}
