#ifndef GRAPH_INTERFACE_H
#define GRAPH_INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
 * GraphInterface
 * --------------
 * A generic interface structure for a graph implementation, holding
 * function pointers for fundamental operations:
 *
 *   add_edge           - Add or modify an edge (src->dest) with a given weight
 *   get_edge           - Retrieve the weight of an edge (src->dest)
 *   calculate_size     - Returns the number of vertices (or another "size" measure)
 *   destroy            - Frees resources
 *
 *   find_cycles              - Finds/Counts cycles (implementation-dependent)
 *   count_hamiltonian_cycles - Counts Hamiltonian cycles
 *   calculate_metric         - Example metric function (you may revise arguments/return)
 *   find_minimal_extension   - Minimum edges needed for Hamiltonian cycle
 *   count_maximal_cycles     - Returns the length of the longest cycle
 *   find_maximal_cycles      - Finds all cycles of maximum length
 *
 * 'vertices' can store the number of vertices in the graph instance.
 */
typedef struct GraphInterfaceTag
{
    /* Basic graph modification/retrieval */
    void (*add_edge)(void* self, int src, int dest, int weight);
    int  (*get_edge)(void* self, int src, int dest);
    int* (*get_all_edges_from_vertex)(void* self, int src, int* out_degree);
    int  (*calculate_size)(void* self);

    int (*get_out_degree)(void* self, int vertex);

    /* Cleanup */
    void (*destroy)(void* self);

    /* Example algorithms (signatures are placeholders—customize as needed) */
    int (*find_cycles)(void* self, int vertices);
    int (*count_hamiltonian_cycles)(void* self, int vertices);

    /* Removed GArray*, replaced with a placeholder function or structure.
       Adjust the signature to match your new data structures or remove if unused. */
    void (*calculate_metric)(void* self, int vertices);

    int (*find_minimal_extension)(void* self, int vertices);
    int (*count_maximal_cycles)(void* self, int vertices);
    int (*find_maximal_cycles)(void* self, int vertices);


    /* Tracks the number of vertices in this graph (optional). */
    int vertices;

} GraphInterface;

#endif // GRAPH_INTERFACE_H
