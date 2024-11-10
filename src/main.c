#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> 
#include "graph.h"
#include "utils.h"
#include "graph_factory.h"
#include "graph_algorithm.h"
#include "config.h"

void handle_arguments(int argc, char *argv[], Config *config);
FILE* open_file_with_retry(const char *filename);
Graph* initialize_graph(const char *filename, FILE *file);
void analyze_graph(Graph *graph);
void cleanup_resources(Graph *graph, FILE *file);

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

Graph* initialize_graph(const char *filename, FILE *file) {
    Graph *graph = create_graph_with_type(SIMPLE_GRAPH, 5);
    if (!graph) {
        fprintf(stderr, "Error creating graph.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    if (!load_graph_from_file(filename)) {
        fprintf(stderr, "Error loading graph from file: %s\n", filename);
        free_graph(graph);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    return graph;
}

void analyze_graph(Graph *graph) {
    printf("Graph size: %d\n", default_algorithm.calculate_size(graph));
    printf("Max cycle count: %d\n", default_algorithm.find_cycles(graph));
    printf("Hamiltonian extensions: %d\n", default_algorithm.count_hamiltonian_cycles(graph));
}

void cleanup_resources(Graph *graph, FILE *file) {
    free_graph(graph);
    fclose(file);
}

int main(int argc, char *argv[]) {
    Config *config = get_config();
    handle_arguments(argc, argv, config);

    FILE *file = open_file_with_retry(config->input_file);
    Graph *graph = initialize_graph(config->input_file, file);

    analyze_graph(graph);
    cleanup_resources(graph, file);

    exit(EXIT_SUCCESS);
}
