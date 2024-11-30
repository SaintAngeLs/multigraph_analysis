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
#include <math.h>
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

void iter_swap(void* first, void* last, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        unsigned char tmp = *(unsigned char*)(first+i);
        *(unsigned char*)(first+i) = *(unsigned char*)(last+i);
        *(unsigned char*)(last+i) = tmp;
    }
}

void reverse(void* first, void* last, size_t size) {
    while (first < last) {
        iter_swap(first, last, size);
        first += size;
        last -= size;
    }
}

// https://en.cppreference.com/mwiki/index.php?title=cpp/algorithm/next_permutation&oldid=126997
int next_permutation(void* first, void* last, size_t size, int (*comp)(const void*, const void*)) {
    if (first == last) return 0;
    void* i = last;
    if (first == (i -= size)) return 0;

    while (1) {
        void* i1, i2;
        i1 = i;
        if (comp(i -= size, i1) < 0) {
            i2 = last;
            while (comp(i, i2 -= size) >= 0)
                ;
            iter_swap(i, i2, size);
            reverse(i1, last, size);
            return 1;
        }
        if (i == first) {
            reverse(first, last);
            return 0;
        }
    }
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
int comp_graphs(const int* a, const int* b) {
    for (int i = 0; i < comp_graphs_n*comp_graphs_n; ++i) {
        if ((a)[i] < (b)[i]) { return -1; }
        if ((a)[i] > (b)[i]) { return 1; }
    }
    return 0;
}

int comp_uint(size_t a, size_t b) {
    return (a < b) ? -1 : ((a > b) ? 1 : 0);
}

int comp_ptr(void* a, void* b) {
    return (b - a) < 0 ? -1 : ((b - a) > 0 ? 1 : 0);
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

int h(int* G, int n, int* degs_self, int* degs_other) {
    for (int i = 0; i < n; ++i) {
        degs_self[i] = 0;
        for (int j = 0; j < n; ++j) {
            degs_self[i] += G[n*i+j];
        }
    }
    heapsort(degs_self, sizeof(int), n, comp_uint);
    int sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += abs(degs_self[i] - degs_other[i]);
    }
    return sum;
}

struct NeighborFactory_ {
    int* orig_graph;
    int n;

    // insert/delete edge
    int curr_out;
    int curr_in;
    int delete_edge_node;

    // perm
    int vertex_permutation_mode;
    int* vertex_perm;

} NeighborFactory;

int nei_factory_init(NeighborFactory* f, int* graph, int n) {
    f->orig_graph = graph;
    f->n = n;
    f->vertex_permutation_mode = 0;
    f->vertex_perm = malloc(sizeof(int)*n);
    if (!f->vertex_perm) return 0;
    
    for (int i = 0; i < n; ++i) {
        f->vertex_perm[i] = i;
    }

    // graph = malloc(sizeof(int)*n*n);
    // (!f->graph) return 0; // TODO
                             //
    //mcpy(f->graph, f->orig_graph, sizeof(int)*n*n);
    f->curr_out = 0;
    f->curr_in = 1;
    f->delete_edge_node = 0;
    return 1;

}

// assume the graph has at least 2 vertices
int next_neighbor(NeighborFactory* f, int** result) {
    if (f->vertex_permutation_node) {
        int perm = next_permutation(f->vertex_perm, f->vertex_perm + n);
        if (!perm) {
            *result = NULL;
            return 1;
        }
        int* graph = malloc(sizeof(int)*n*n);
        if (!graph) return 0;
        for (int i = 0; i < n; ++i) {
            memcpy(&graph[f->vertex_perm[i]*n], &f->orig_graph[i*n], sizeof(int)*n);
        }
        *result = graph;
    } else {
        if (delete_edge_node) {
            if (f->orig_graph[f->n*f->curr_out + f->curr_in]) {
                int* graph = malloc(sizeof(int)*n*n);
                if (!graph) return 0;
                memcpy(graph, f->orig_graph, sizeof(int)*n*n);
                --graph[f->n*f->curr_out + f->curr_in];
                *result = graph;
            }
        } else {
            int* graph = malloc(sizeof(int)*n*n);
            if (!graph) return 0;
            memcpy(graph, f->orig_graph, sizeof(int)*n*n);
            ++graph[f->n*f->curr_out + f->curr_in];
            *result = graph;
        }
        if (delete_edge_node) {
            ++f->curr_in;
            if (f->curr_in == n) {
                f->curr_in = 0;
                ++f->curr_out;
                if (f->curr_out == n) {
                    f->vertex_permutation_mode = 1;
                }
            }
        }
        delete_edge_node = !delete_edge_node;
    }

    return 1;
}

void nei_factory_free(NeighborFactory* f) {
    free(f->vertex_perm);
}

int a_star(int* start, int* goal, int n, int* result) {
    /* put start and goal to the db */
    comp_graphs_n = n;
    
    AvlNode* tmp_avl_alloc = malloc(sizeof(AvlNode));
    //if (!tmp_avl_alloc) {
        // TODO
    //}
    int* tmp_graph = malloc(sizeof(int)*n*n);
    if (!tmp_graph) {
    //    return 0; // TODO
    }
    memcpy(tmp_graph, start, sizeof(int)*n*n);
    AvlNode* avl_graph_storage = avl_create_node(tmp_avl_alloc, tmp_graph, NULL);
    
    tmp_avl_alloc = malloc(sizeof(AvlNode));
    if (!tmp_avl_alloc) {
        // TODO
    }
    tmp_graph = malloc(sizeof(int)*n*n);
    if (!tmp_graph) {
    //    return 0; // TODO
    }
    memcpy(tmp_graph, goal, sizeof(int)*n*n);
    avl_insert(&avl_graph_storage, tmp_avl_alloc, tmp_graph, NULL, comp_graphs);

    AvlNode* avl_qprior = NULL; // <priority, graph node>
    AvlNode* avl_qgraph = NULL; // <graph node, priority map node>
    AvlNode* avl_g = NULL;      // <graph node, cost>
    AvlNode* avl_c = NULL;      // <graph node>
    
    // calculate degs
    int *degs_goal, *degs_curr;
    degs_goal = malloc(sizeof(int)*n); // TODO
    degs_curr = malloc(sizeof(int)*n); // TODO
    
    // TODO: dry
    for (int i = 0; i < n; ++i) {
        degs_goal[i] = 0;
        for (int j = 0; j < n; ++j) {
            degs_goal[i] += goal[n*i+j];
        }
    }
    heapsort(degs_goal, sizeof(int), n, comp_uint);

    // insert start
    tmp_avl_alloc = malloc(sizeof(AvlNode)); // TODO
    AvlNode* start_graph_node = avl_find(avl_graph_storage, start, comp_graphs);
    AvlNode* goal_graph_node = avl_find(avl_graph_storage, goal, comp_graphs);
    avl_insert(&avl_qprior, tmp_avl_alloc, h(start, n, degs_curr, degs_goal), start_graph_node, comp_uint);
    AvlNode* startPriorityNode = tmp_avl_alloc;

    tmp_avl_alloc = malloc(sizeof(AvlNode)); // TODO
    avl_insert(&avl_g, tmp_avl_alloc, start_graph_node, 0, comp_ptr);

    // TODO
    tmp_avl_alloc = malloc(sizeof(AvlNode));
    avl_insert(&avl_qgraph, tmp_avl_alloc, start_graph_node, startPriorityNode, comp_ptr);
    
    while (avl_qprior) {
        AvlNode* min_priority_node = avl_min_node(avl_qprior);
        AvlNode* current_node = min_priority_node->value;
        free(avl_delete_key(&avl_qprior, min_priority_node->key, comp_uint));
        if (current_node == goal_graph_node) {
            // TODO
            return avl_find(avl_g, current_node, comp_ptr)->value;
        }
        tmp_avl_alloc = malloc(sizeof(AvlNode));
        avl_insert(&avl_c, tmp_avl_alloc, current_node, NULL, comp_ptr);
        NeighborFactory factory;
        if (!nei_factory_init(&factory, current_node->key, n)) {
            // TODO
        }

        int* neighbor;
        while (1) {
            int d = !factory->vertex_permutation_mode;

            if (!next_neighbor(&factory, &neighbor)) {
                break; // TODO
            }

            if (!neighbor) {
                break;
            }
    
            AvlNode* neighbor_node = avl_find(&avl_graph_storage, neighbor, comp_graph);
            if (!neighbor_node) {
                tmp_avl_alloc = malloc(sizeof(AvlNode));
                if (!tmp_avl_alloc) {
                    // TODO
                }
                avl_insert(&avl_graph_storage, tmp_avl_alloc, neighbor, NULL, comp_graphs);
                neighbor_node = tmp_avl_alloc;
            }

            if (avl_find(avl_c, neighbor_node, comp_ptr)) {
                continue;
            }

            int cost = d + avl_find(avl_g, current_node, comp_ptr)->value;
            AvlNode* g_nei_node = avl_find(avl_g, neighbor_node, comp_ptr);
            AvlNode* qg_nei_node = avl_find(avl_qgraph, neighbor_node, comp_ptr);
            if (qg_nei_node && (!g_nei_node || cost < g_nei_node->value)) {
                free(avl_delete_key(&avl_qprior, qg_nei_node->value->key, comp_ptr));
                free(avl_delete_key(&avl_qgraph, neighbor_node, comp_ptr));
                qg_nei_node = NULL;
            }
            if (!qg_nei_node) {
                if (!g_nei_node) {
                    tmp_avl_alloc = malloc(sizeof(AvlNode));
                    if (!tmp_avl_alloc) {
                        // TODO
                    } 
                    avl_insert(&avl_g, tmp_avl_alloc, neighbor_node, cost, comp_ptr);
                } else {
                    g_nei_node->value = cost;
                }
                tmp_avl_alloc = malloc(sizeof(AvlNode));
                if (!tmp_avl_alloc) {
                    // TODO
                } 
                AvlNode* priority_node = tmp_avl_alloc;
                avl_insert(&avl_qprior, tmp_avl_alloc, cost + h(neighbor, n, degs_curr, degs_goal), neighbor_node, comp_uint);
                tmp_avl_alloc = malloc(sizeof(AvlNode));
                if (!tmp_avl_alloc) {
                    // TODO
                }
                avl_insert(&avl_qgraph, tmp_avl_alloc, neighbor_node, priority_node, comp_ptr);
            }
        }
        
        nei_factory_free(&factory);
    }

    free(degs_goat);
    free(degs_curr);
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


