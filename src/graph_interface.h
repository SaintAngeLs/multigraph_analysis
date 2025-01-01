#ifndef GRAPH_INTERFACE_H
#define GRAPH_INTERFACE_H

#include <glib.h>

typedef struct GraphInterface {

    void (*add_edge)(void *self, int src, int dest, int weight);

    int (*get_edge)(void *self, int src, int dest);

    int (*calculate_size)(void *self);

    void (*destroy)(void *self);

    int (*find_cycles)(void *self, int vertices);

    int (*count_hamiltonian_cycles)(void *self, int vertices);

    GArray* (*calculate_metric)(void *self, int vertices);

    int (*find_minimal_extension)(void *self, int vertices);

    int (*count_maximal_cycles)(void *graph, int vertices);

    int (*find_maximal_cycles)(void *graph, int vertices);
    
    int vertices;

} GraphInterface;

#endif // GRAPH_INTERFACE_H
