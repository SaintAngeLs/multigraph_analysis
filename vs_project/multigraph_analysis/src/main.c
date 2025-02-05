#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>
#include <windows.h>  

#include "graph.h"
#include "utils.h"
#include "graph_interface.h"
#include "../include/config.h"
#include "common_utils.h"
#include "stack.h"   
#include "matching.h"

#include "graph_algorithm_context.h"
#include "cycle_list.h"
#include "string_list.h"
#include "graph_algorithm.h"
#include "method_wrapper.h"
#include "time.h"

#define THRESHOLD     10      /* If vertices >= THRESHOLD, use approximate */
#define MAX_ITER      20000   /* For approximate sim-anneal logic */
#define INITIAL_TEMP  200.0
#define COOLING_RATE  0.99

 
typedef struct GraphAlgorithmTag {
    int  (*calculate_size)(void* graph);
    int  (*find_cycles)(void* graph, int vertices,
        int*** output_cycles, int** cycle_sizes, int* cycle_count);
    int  (*count_hamiltonian_cycles)(void* graph, int vertices,
        int*** output_cycles, int** cycle_sizes, int* cycle_count);
    void (*calculate_metric)(void* g1, int v1, void* g2, int v2,
        int* exact_metric, int* approximate_metric);
    int (*get_out_degree)(void* self, int vertex);
    int  (*find_minimal_extension)(void* graph, int vertices);
    int  (*count_maximal_cycles)(void* graph, int vertices);
    int  (*find_maximal_cycles)(void* graph, int vertices,
        int*** output_cycles, int** cycle_sizes, int* cycle_count);
} GraphAlgorithm;


static GraphAlgorithm default_algorithm = {
    .calculate_size = calculate_size_wrapper,
    .find_cycles = find_cycles_wrapper,
    .count_hamiltonian_cycles = count_hamiltonian_cycles_wrapper,
    .calculate_metric = calculate_metric_wrapper,
    .find_minimal_extension = find_minimal_extension_wrapper,
    .count_maximal_cycles = count_maximal_cycles_wrapper,
    .find_maximal_cycles = find_maximal_cycles_wrapper
};

/* argument handling + open_file_with_retry */
void handle_arguments(int argc, char* argv[], Config* config) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    config->input_file = argv[1];
    if (argc == 3) {
        config->metric_optional_file = argv[2];
    }
}
FILE* open_file_with_retry(const char* filename) {
    FILE* file;
    do {
        file = fopen(filename, "r");
    } while (!file && errno == EINTR);
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    return file;
}

/*
 * analyze_multigraph
 *    1) All cycles
 *    2) Hamiltonian cycles
 *    3) Minimal extension for Hamiltonian cycle
 *    4) Maximal cycle length
 *    5) All cycles of that maximal length
 */
extern GraphAlgorithm default_algorithm;  /* Our global struct above */

void analyze_multigraph(GraphInterface* multigraph) {
    printf("Analyzing Multigraph:\n");
    printf("------------------------------------------------\n");

    int graph_size = default_algorithm.calculate_size(multigraph);
    printf("Graph size: %d\n", graph_size);

    /* 1) All cycles */
    {
        int** cyc_out = NULL;
        int* cyc_sizes = NULL;
        int cyc_count_num = 0;
        int cyc_count = default_algorithm.find_cycles(
            multigraph,
            multigraph->vertices,
            &cyc_out,
            &cyc_sizes,
            &cyc_count_num
        );
        printf("All cycles: %d\n", cyc_count);
        print_cycles(multigraph, cyc_out, cyc_sizes, cyc_count);
    }

    /* 2) Hamiltonian cycles */
    {
        int** ham_out = NULL;
        int* ham_sizes = NULL;
        int ham_count_num = 0;
        int ham_count = default_algorithm.count_hamiltonian_cycles(
            multigraph,
            multigraph->vertices,
            &ham_out,
            &ham_sizes,
            &ham_count_num
        );
        printf("Hamiltonian cycles: %d\n", ham_count);
        print_cycles(multigraph, ham_out, ham_sizes, ham_count);
    }

    /* 3) minimal extension for Hamiltonian cycle */
    int min_ext = default_algorithm.find_minimal_extension(multigraph, multigraph->vertices);
    printf("Minimal extension for Hamiltonian cycle: %d\n", min_ext);

    /* 4) maximal cycle length */
    int max_len = default_algorithm.count_maximal_cycles(multigraph, multigraph->vertices);
    printf("Maximal cycle length: %d\n", max_len + 1);

    /* 5) find all cycles of that length */
    {
        int** max_out = NULL;
        int* max_sizes = NULL;
        int max_count_var = 0;
        int max_count = default_algorithm.find_maximal_cycles(
            multigraph,
            multigraph->vertices,
            &max_out,
            &max_sizes,
            &max_count_var
        );
        printf("Maximal cycles: %d\n", max_count);
        print_cycles(multigraph, max_out, max_sizes, max_count);
    }

    printf("------------------------------------------------\n\n");
}

void process_multigraphs(const char* file_name, GraphArray* multigraphs_to_compare) {
    FILE* file = open_file_with_retry(file_name);
    static int file_counter = 0;
    printf("Processing file %d: %s\n", ++file_counter, file_name);

    int num_graphs;
    fscanf(file, "%d", &num_graphs);
    printf("Processing %d graphs.\n\n", num_graphs);

    for (int i = 0; i < num_graphs; i++) {
        printf("Graph %d:\n", i + 1);
        int vertices;
        fscanf(file, "%d", &vertices);

        GraphInterface* multigraph = create_multigraph(vertices);
        if (!multigraph) {
            fprintf(stderr, "Failed to initialize graph interface for graph %d.\n", i + 1);
            continue;
        }

        if (i == 0) {
            add_graph(multigraphs_to_compare, multigraph);
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
    }
    fclose(file);
}

/* Compare two loaded graphs for metrics */
void process_metrics(GraphInterface* multigraph_1, GraphInterface* multigraph_2) {
    printf("Comparing graphs:\n");
    printf("------------------------------------------------\n");

    printf("First graph with '%d' vertices and '%d' edges\n",
        multigraph_1->vertices,
        multigraph_1->calculate_size(multigraph_1));
    printf("Second graph with '%d' vertices and '%d' edges\n",
        multigraph_2->vertices,
        multigraph_2->calculate_size(multigraph_2));

    int exact_metric, approximate_metric;
    default_algorithm.calculate_metric(
        multigraph_1, multigraph_1->vertices,
        multigraph_2, multigraph_2->vertices,
        &exact_metric,
        &approximate_metric
    );
    printf("Graph similarity metric (exact): %d\n", exact_metric);
    printf("Graph similarity metric (approx): %d\n", approximate_metric);
    printf("------------------------------------------------\n\n");
}

extern Config* get_config();

int main(int argc, char* argv[]) {
    Config* config = get_config();
    handle_arguments(argc, argv, config);

    GraphArray multigraphs_to_compare;
    init_graph_array(&multigraphs_to_compare, 2);

    process_multigraphs(config->input_file, &multigraphs_to_compare);

    if (config->metric_optional_file) {
        process_multigraphs(config->metric_optional_file, &multigraphs_to_compare);
        if (multigraphs_to_compare.size >= 2) {
            process_metrics(multigraphs_to_compare.data[0],
                multigraphs_to_compare.data[1]);
        }
    }

    free_graph_array(&multigraphs_to_compare);
    return EXIT_SUCCESS;
}
