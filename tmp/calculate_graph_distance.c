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

static int comp_graphs_n;
int comp_graphs(const int*const* a, const int*const* b) {
    for (int i = 0; i < comp_graphs_n*comp_graphs_n; ++i) {
        if ((*a)[i] < (*b)[i]) { return -1; }
        if ((*a)[i] > (*b)[i]) { return 1; }
    }
    return 0;
}

void avl_for_each(AvlNode* root, void (*op)(AvlNode* node)) {
    if (!root) return;
    op(root);
    avl_for_each(root->left, op);
    avl_for_each(root->right, op);
}

void avl_free(AvlNode* root) {
    if (!root) return;
    avl_free(root->left);
    avl_free(root->right);
    free(root);
}

void free_graph_storage_unit(AvlNode* node) {
    free((void*)node->key);
}

int a_star(int* start, int* goal, int n, int* result) {
    /* put start and goal to the db */
    comp_graphs_n = n;
    
    AvlNode* tmp_avl_alloc = malloc(sizeof(AvlNode));
    if (!tmp_avl_alloc) {
        // TODO
    }
    int* tmp_graph = malloc(sizeof(int), n*n);
    if (!tmp_graph) {
        return 0; // TODO
    }
    memcpy(tmp_graph, start, sizeof(int)*n*n);
    AvlNode* avl_graph_storage = avl_create_node(tmp_avl_alloc, tmp_graph, NULL);
    
    tmp_avl_alloc = malloc(sizeof(AvlNode));
    if (!tmp_avl_alloc) {
        // TODO
    }
    tmp_graph = malloc(sizeof(int), n*n);
    if (!tmp_graph) {
        return 0; // TODO
    }
    memcpy(tmp_graph, goal, sizeof(int)*n*n);
    avl_insert(&avl_graph_storage, tmp_avl_alloc, tmp_graph, NULL, comp_graphs);

    AvlNode* avl_qprior = NULL;
    AvlNode* avl_qgraph = NULL;
    AvlNode* avl_g = NULL;
    AvlNode* avl_c = NULL;

    // TODO: continue
    
    avl_free(avl_c);
    avl_free(avl_g);
    avl_free(avl_qgraph);
    avl_free(avl_qprior);
    avl_for_each(avl_graph_storage, free_graph_storage_unit);
    avl_free(avl_graph_storage);
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

/*
typedef struct MemoryNode_ {
    AvlNode avlnode;
    struct MemoryNode_* prev;
    struct MemoryNode_* next;

} MemoryNode;

void free_memory_list(MemoryNode* node) {
    while (!node) {
        MemoryNode* next = node->next;
        free(node);
        node = next;
    }
}

struct MemoryList {
    MemoryNode* head;
    MemoryNode* curr;
};

int memory_list_append() {
    
}

int memory_list_remove() {

}

*/


