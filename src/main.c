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

void print_cycles(GArray *output_cycles) {
    printf("Found %u cycles:\n", output_cycles->len);
    
    for (guint i = 0; i < output_cycles->len; i++) {
        GArray *cycle = g_array_index(output_cycles, GArray *, i);
        
        for (guint j = 0; j < cycle->len; j++) {
            printf("%d", g_array_index(cycle, int, j));
            if (j < cycle->len - 1) {
                printf(" - ");
            }
        }
        printf("\n");
        
        g_array_free(cycle, TRUE);
    }
}

void analyze_multigraph(GraphInterface *multigraph) {
    printf("Analyzing Multigraph:\n");
    printf("------------------------------------------------\n");

    int graph_size = multigraph->calculate_size(multigraph);
    printf("Graph size (number of edges): %d\n", graph_size);

    GArray *output_cycles = g_array_new(FALSE, FALSE, sizeof(GArray *));
    default_algorithm.find_cycles(multigraph, multigraph->vertices, output_cycles);
    print_cycles(output_cycles);
    g_array_free(output_cycles, TRUE);

    int hamiltonian_cycles = default_algorithm.count_hamiltonian_cycles(multigraph, multigraph->vertices);
    printf("Hamiltonian cycles: %d\n", hamiltonian_cycles);

    GArray *metric = default_algorithm.calculate_metric(multigraph, multigraph->vertices);
    printf("Metric size: %d\n", metric->len);
    printf("Smallest resolving set:\n");
    for (int i = 1; i <= metric->len; i++) {
        printf(" %d) = %d\n", i, g_array_index(metric, int, i - 1));
    }
    g_array_free(metric, TRUE);

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

        analyze_multigraph(multigraph);
        cleanup_resources(multigraph);
    }

    fclose(file);
    return EXIT_SUCCESS;
}
