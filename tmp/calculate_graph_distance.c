/* Graphs are represented as one-dimensional arrays.
 * graph[n*i + j] means an edge from i to j
 *
 */

/* 
 *
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include "avl.h"

void swap_int_ptr(int** a, int** b) {
    int* tmp = *a;
    *a = *b;
    *b = tmp;
}

void swap_int(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int prepare_graph(int Xn, int n, int** X, int** Xparam) {
    if (Xn == n) {
        *X = *Xparam;
    } else {
        *X = malloc(sizeof(int) * n);
        if (!*X) {
            return 0;
        }
        for (int i = 0; i < Xn; ++i) {
            for (int j = 0; j < Xn; ++j) {
                (*X)[n*i + j] = (*Xparam)[Xn*i + j];
            }
            for (int j = Xn; j < n; ++j) {
                (*X)[n*i + j] = 0;
            }
        }
        for (int i = Xn; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                (*X)[n*i + j] = 0;
            }
        }
    }
    return 1;
}

void free_graph(int* X, int* Xparam) {
    if (Xparam != X) {
        free(X);
    }
}

typedef struct MemoryNode {
    GraphStorageNode value;
    struct MemoryNode* prev;
    struct MemoryNode* next;

} MemoryNode;

void free_memory_list_gstor(MemoryNode* node) {
    while (!node) {
        MemoryNode* next = node->next;
        free(node->value.value);
        free(node);
        node = next;
    }
}

static int comp_graphs_n;
int comp_graphs(int** a, int** b) {
    for (int i = 0; i < comp_graphs_n*comp_graphs_n; ++i) {
        if ((*a)[i] < (*b)[i]) { return -1; }
        if ((*a)[i] > (*b)[i]) { return 1; }
    }
    return 0;
}

int a_star(int* start, int* goal, int n, int* result) {
    MemoryNode* memory_list_head = malloc(sizeof(MemoryNode));
    if (!memory_list_head) return 0;
    memory_list_head->prev = NULL;
    memory_list_head->next = NULL;
    MemoryNode* memory_list_curr = memory_list_head;
    
    // TODO
    MemoryNode* memory_qprior_head;
    MemoryNode* memory_qgraph_head;
    MemoryNode* memory_g_head;
    MemoryNode* memory_c_head;

    MemoryNode* memory_qprior_curr;
    MemoryNode* memory_qgraph_curr;
    MemoryNode* memory_g_curr;
    MemoryNode* memory_c_curr;

    GraphStorageNode* graph_storage_root = NULL;
    
    /* put start and goal to the db */
    comp_graphs_n = n;
    
    // TODO: repeated
    // difference: start, goal
    graph_storage_root = avl_insert_gstor(graph_storage_root, tmp_graph, NULL, &memory_list_curr->value, comp_graphs);
    
    memory_list_curr->next = malloc(sizeof(MemoryNode));
    if (!memory_list_curr->next) {
        /*TODO*/
        return 0;
    }
    memory_list_curr->next->prev = memory_list_curr;
    memory_list_curr = memory_list_curr->next;
    memory_list_curr->next = NULL;
   
    int* tmp_graph = malloc(sizeof(int), n*n);
    if (!tmp_graph) {
        return 0; // TODO
    }
    memcpy(tmp_graph, goal, n*n);
     
    graph_storage_root = avl_insert_gstor(graph_storage_root, tmp_graph, NULL, &memory_list_curr->value, comp_graphs);
    
    memory_list_curr->next = malloc(sizeof(MemoryNode));
    if (!memory_list_curr->next) {
        /*TODO*/
        return 0;
    }
    memory_list_curr->next->prev = memory_list_curr;
    memory_list_curr = memory_list_curr->next;
    memory_list_curr->next = NULL;
    
    // TODO
    GraphStorageNode* qprior_root = NULL;
    GraphStorageNode* qgraph_root = NULL;
    GraphStorageNode* g_root = NULL;
    GraphStorageNode* c_root = NULL;
    
    
    qprior_root = avl_insert_gstor(qprior_root, );





    // TODO
    free_memory_list(memory_c_head);
    free_memory_list(memory_g_head);
    free_memory_list(memory_qgraph_head);
    free_memory_list(memory_qprior_head);
    free_memory_list_gstor(memory_list_head);
    return 1;
}

/* A and B pointers are not the same! */
int calculate_graph_distance(int* Aparam, int An, int* Bparam, int Bn, int* result) {
    int success = 0;
    int n = (An > Bn ? An : Bn);
    int *A, *B;

    if (!prepare_graph(An, n, &A, &Aparam)) {
        goto A_alloc_failed;
    }

    if (!prepare_graph(Bn, n, &B, &Bparam)) {
        goto B_alloc_failed;
    }
    
    if (a_star(A, B, n, result)) {
        success = 1;
    }

    free_graph(B, Bparam);

B_alloc_failed:

    free_graph(A, Aparam);

A_alloc_failed:
    
    return success;
}

int main() {
    return EXIT_SUCCESS;
}
