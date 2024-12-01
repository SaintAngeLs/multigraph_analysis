#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "graph.h"
#include "utils.h"
#include "config.h"
#include "graph_algorithm.h"

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
    printf("Analyzing Multigraph:\n");
    printf("------------------------------------------------\n");

    int graph_size = multigraph->calculate_size(multigraph);
    printf("Graph size (number of edges): %d\n", graph_size);

    int cycle_count = default_algorithm.find_cycles(multigraph, multigraph->vertices);
    printf("Cycle count: %d\n", cycle_count);

    int hamiltonian_cycles = default_algorithm.count_hamiltonian_cycles(multigraph, multigraph->vertices);
    printf("Hamiltonian cycles: %d\n", hamiltonian_cycles);

    int minimal_extension = default_algorithm.find_minimal_extension(multigraph, multigraph->vertices);
    printf("Minimal extension for Hamiltonian cycle: %d\n", minimal_extension);

    int maximal_cycle_length = default_algorithm.find_maximal_cycles(multigraph, multigraph->vertices);
    printf("Maximal cycle length: %d\n", maximal_cycle_length);

    int maximal_cycle_count = default_algorithm.count_maximal_cycles(multigraph, multigraph->vertices);
    printf("Number of maximal cycles: %d\n", maximal_cycle_count);

    printf("------------------------------------------------\n\n");
}

void cleanup_resources(GraphInterface *multigraph) {
    if (multigraph) {
        multigraph->destroy(multigraph);
    }
}

int main(int argc, char *argv[]) {
    Config *config = get_config();
    handle_arguments(argc, argv, config);

    FILE *file = open_file_with_retry(config->input_file);

    int num_graphs;
    fscanf(file, "%d", &num_graphs);
    printf("Processing %d graphs.\n\n", num_graphs);

    GArray *graphs = g_array_new(FALSE, FALSE, sizeof(GraphInterface *));

    for (int i = 0; i < num_graphs; i++) {
        printf("Graph %d:\n", i + 1);

        int vertices;
        fscanf(file, "%d", &vertices);

        GraphInterface *multigraph = create_multigraph(vertices);
        if (!multigraph) {
            fprintf(stderr, "Failed to initialize the graph interface for graph %d.\n", i + 1);
            continue;
        }

        for (int j = 0; j < vertices; j++) {
            for (int k = 0; k < vertices; k++) {
                int weight;
                fscanf(file, "%d", &weight);
                if (weight > 0) { 
                    multigraph->add_edge(multigraph, j, k, weight);
                }
            }
        }

        g_array_append_val(graphs, multigraph);
        analyze_multigraph(multigraph);
    }

    for (int i = 1; i < graphs->len; i++) {
        GraphInterface *multigraph_1 = g_array_index(graphs, GraphInterface *, i-1);
        GraphInterface *multigraph_2 = g_array_index(graphs, GraphInterface *, i);
        double metric = default_algorithm.calculate_metric(multigraph_1, multigraph_1->vertices, multigraph_2, multigraph_2->vertices);
        printf("Graph similarity metric between graph %d and graph %d: %.3f\n", i, i+1, metric);
    }

    for (int i = 0; i < graphs->len; i++) {
        GraphInterface *multigraph = g_array_index(graphs, GraphInterface *, i);
        cleanup_resources(multigraph);
    }
    g_array_free(graphs, TRUE);

    fclose(file);
    return EXIT_SUCCESS;
}
