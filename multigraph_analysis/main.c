#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "glib.h"
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

    if(argc == 3){
        config->metric_optional_file = argv[2];
    }
}


FILE* open_file_with_retry(const char* filename) {
    FILE* file;
    errno_t err;
    do {
        err = fopen_s(&file, filename, "r");
    } while (!file && err != 0);

    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    return file;
}

void print_cycles(GArray *output_cycles) {
    for (int i = 0; i < output_cycles->len; i++) {
        GArray *cycle = g_array_index(output_cycles, GArray *, i);
        
        for (int j = 0; j < cycle->len; j++) {
            printf("%d", g_array_index(cycle, int, j));
            if (j < cycle->len - 1) {
                printf(" -> ");
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
    printf("All cycles: %u\n", output_cycles->len);
    print_cycles(output_cycles);
    g_array_free(output_cycles, TRUE);

    GArray *output_hamiltonian_cycles = g_array_new(FALSE, FALSE, sizeof(GArray *));
    default_algorithm.count_hamiltonian_cycles(multigraph, multigraph->vertices, output_hamiltonian_cycles);
    printf("Hamiltonian cycles: %d\n", output_hamiltonian_cycles->len);
    print_cycles(output_hamiltonian_cycles);
    g_array_free(output_hamiltonian_cycles, TRUE);

    int minimal_extension = default_algorithm.find_minimal_extension(multigraph, multigraph->vertices);
    printf("Minimal extension for Hamiltonian cycle: %d\n", minimal_extension);

    int maximal_cycle_length = default_algorithm.count_maximal_cycles(multigraph, multigraph->vertices);
    printf("Maximal cycle length: %d\n", maximal_cycle_length);

    GArray *output_maximal_cycles = g_array_new(FALSE, FALSE, sizeof(GArray *));
    int maximal_cycle_count = default_algorithm.find_maximal_cycles(multigraph, multigraph->vertices, output_maximal_cycles);
    printf("Maximal cycles: %u\n", maximal_cycle_count);
    print_cycles(output_maximal_cycles);
    g_array_free(output_maximal_cycles, TRUE);

    printf("------------------------------------------------\n\n");
}

void cleanup_resources(GraphInterface *multigraph) {
    if (multigraph) {
        multigraph->destroy(multigraph);
    }
}

void process_multigraphs(const char* file_name, GArray* multigraphs_to_compare) {
    FILE *file = open_file_with_retry(file_name);
    static int file_counter = 0;
    printf("Processing file %d: %s\n", ++file_counter, file_name);

    int num_graphs;
    fscanf_s(file, "%d", &num_graphs);
    printf("Processing %d graphs.\n\n", num_graphs);

    for (int i = 0; i < num_graphs; i++) {
        printf("Graph %d:\n", i + 1);

        int vertices;
        fscanf_s(file, "%d", &vertices);

        GraphInterface *multigraph = create_multigraph(vertices);
        if (!multigraph) {
            fprintf(stderr, "Failed to initialize the graph interface for graph %d.\n", i + 1);
            continue;
        }

        if(i == 0){
            g_array_append(multigraphs_to_compare, multigraph);
        }

        for (int j = 0; j < vertices; j++) {
            for (int k = 0; k < vertices; k++) {
                int weight;
                fscanf_s(file, "%d", &weight);
                if (weight > 0) {
                    multigraph->add_edge(multigraph, j, k, weight);
                }
            }
        }

        analyze_multigraph(multigraph);
    }

    fclose(file);
}

void process_metrics(GraphInterface* multigraph_1, GraphInterface* multigraph_2){
    printf("Comparing graphs:\n");
    printf("------------------------------------------------\n");

    printf("First graph with '%d' vertices and '%d' edges\n", multigraph_1->vertices, multigraph_1->calculate_size(multigraph_1));
    printf("Second graph with '%d' vertices and '%d' edges\n", multigraph_2->vertices, multigraph_2->calculate_size(multigraph_2));

    int exact_metric, approximate_metric;
    default_algorithm.calculate_metric(multigraph_1, multigraph_1->vertices, multigraph_2, multigraph_2->vertices, &exact_metric, &approximate_metric);
    printf("Graph similarity metric between graphs: %d\n",exact_metric);
    printf("Approximate graph similarity metric between graphs: %d\n",approximate_metric);

    printf("------------------------------------------------\n\n");
}

int main(int argc, char *argv[]) {
    Config *config = get_config();
    handle_arguments(argc, argv, config);

    GArray *multigraphs_to_compare = g_array_new(FALSE, FALSE, sizeof(GraphInterface *));

    process_multigraphs(config->input_file, multigraphs_to_compare);
    if(config->metric_optional_file){
        process_multigraphs(config->metric_optional_file, multigraphs_to_compare);
        process_metrics(g_array_index(multigraphs_to_compare, GraphInterface *, 0), g_array_index(multigraphs_to_compare, GraphInterface *, 1));
    }

    g_array_free(multigraphs_to_compare, TRUE);
    return EXIT_SUCCESS;
}
