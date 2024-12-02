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
#include "heap_sort.h"
#include "two_three_heap.h"

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
        unsigned char tmp = ((unsigned char*)(first))[i];
        ((unsigned char*)(first))[i] = ((unsigned char*)(last))[i];
        ((unsigned char*)(last))[i] = tmp;
    }
}

void reverse(void* first, void* last, size_t size) {
    last -= size;
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
        void* i1, *i2;
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
            reverse(first, last, size);
            return 0;
        }
    }
}

typedef struct MemoryNode_ {
    void* mem;
    struct MemoryNode_* prev;

} MemoryNode;

int bulk_alloc(MemoryNode** memnode, size_t size) {
    MemoryNode* newmem = malloc(sizeof(MemoryNode));
    if (!newmem) return 0;
    newmem->mem = malloc(size);
    if (!newmem->mem) {
        free(newmem);
        return 0;
    }
    newmem->prev = *memnode;
    *memnode = newmem;
    return 1;
}

void bulk_free(MemoryNode* memnode) {
    while (memnode) {
        free(memnode->mem);
        MemoryNode* prev = memnode->prev;
        free(memnode);
        memnode = prev;
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

static int comp_graph_n;
int comp_graph(size_t ap, size_t bp) {
    int* a = (int*)ap;
    int* b = (int*)bp;
    for (int i = 0; i < comp_graph_n*comp_graph_n; ++i) {
        if ((a)[i] < (b)[i]) { return -1; }
        if ((a)[i] > (b)[i]) { return 1; }
    }
    return 0;
}

int comp_int_ptr(const void* ap, const void* bp) {
    const int *a = ap;
    const int *b = bp;
    return (*a < *b) ? -1 : (*a > *b ? 1 : 0);
}

int comp_uint(size_t a, size_t b) {
    return (a < b) ? -1 : ((a > b) ? 1 : 0);
}

int comp_ptr(size_t a, size_t b) {
    return (a < b) ? -1 : ((a > b) ? 1 : 0);
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


const int permutation_cost = 0;
const int swap_cost = 10000;
const int add_remove_cost = 1;


int h(int* G, int n, int* degs_self, int* degs_other) {
    if (swap_cost < 2 * add_remove_cost) {
        return 0;
    }

    for (int i = 0; i < n; ++i) {
        degs_self[i] = 0;
        for (int j = 0; j < n; ++j) {
            degs_self[i] += G[n*i+j];
        }
    }
    heapsort(degs_self, sizeof(int), n, comp_int_ptr);
    int sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += add_remove_cost * abs(degs_self[i] - degs_other[i]);
    }
    return sum;
}

enum NeighborFactoryMode {
    NEI_BEGIN,
    NEI_ADD = NEI_BEGIN,
    NEI_REMOVE,
    NEI_SWAP,
    NEI_PERM,
    NEI_END
};

typedef struct NeighborFactory_ {
    int* orig_graph;
    int n;

    // insert/delete edge
    int curr_out;
    int curr_in;

    // perm
    enum NeighborFactoryMode mode;
    int* vertex_perm;

} NeighborFactory;

int nei_factory_init(NeighborFactory* f, int* graph, int n) {
    f->orig_graph = graph;
    f->n = n;
    f->mode = 0;
    f->vertex_perm = malloc(sizeof(int)*n);
    if (!f->vertex_perm) return 0;
    
    for (int i = 0; i < n; ++i) {
        f->vertex_perm[i] = i;
    }

    // graph = malloc(sizeof(int)*n*n);
    // (!f->graph) return 0; // 
                             //
    //mcpy(f->graph, f->orig_graph, sizeof(int)*n*n);
    f->curr_out = 0;
    f->curr_in = 1;
    f->mode = NEI_ADD;
    return 1;

}



// assume the graph has at least 2 vertices
int next_neighbor(NeighborFactory* f, int** result) {
    int done = 0;
    while (!done) {
        if (f->mode == NEI_PERM) {
            int perm = next_permutation(f->vertex_perm, f->vertex_perm + f->n, sizeof(int), comp_int_ptr);
            if (!perm) {
                *result = NULL;
                return 1;
            }
            int* graph = malloc(sizeof(int)*f->n*f->n);
            if (!graph) return 0;
            for (int i = 0; i < f->n; ++i) {
                for (int j = 0; j < f->n; ++j) {
                    graph[f->vertex_perm[i]*f->n + f->vertex_perm[j]] = f->orig_graph[i*f->n + j];
                }
                //memcpy(&graph[f->vertex_perm[i]*f->n], &f->orig_graph[i*f->n], sizeof(int)*f->n);
            }
            *result = graph;
            done = 1;
        } else {
            if (f->mode == NEI_REMOVE) {
                if (f->orig_graph[f->n*f->curr_out + f->curr_in]) {
                    int* graph = malloc(sizeof(int)*f->n*f->n);
                    if (!graph) return 0;
                    memcpy(graph, f->orig_graph, sizeof(int)*f->n*f->n);
                    --graph[f->n*f->curr_out + f->curr_in];
                    *result = graph;
                    done = 1;
                }
            } else if (f->mode == NEI_SWAP) {
                if (f->orig_graph[f->n*f->curr_out + f->curr_in] !=
                    f->orig_graph[f->n*f->curr_in + f->curr_out]) {
                
                    int* graph = malloc(sizeof(int)*f->n*f->n);
                    if (!graph) return 0;
                    memcpy(graph, f->orig_graph, sizeof(int)*f->n*f->n);
                    swap_int(&graph[f->n*f->curr_out + f->curr_in], &graph[f->n*f->curr_in + f->curr_out]);
                    *result = graph;
                    done = 1;
                }
            } else {
                int* graph = malloc(sizeof(int)*f->n*f->n);
                if (!graph) return 0;
                memcpy(graph, f->orig_graph, sizeof(int)*f->n*f->n);
                ++graph[f->n*f->curr_out + f->curr_in];
                *result = graph;
                done = 1;
            }
            if (f->mode == NEI_REMOVE) {
                ++f->curr_in;
                if (f->curr_in == f->n) {
                    f->curr_in = 0;
                    ++f->curr_out;
                    if (f->curr_out == f->n) {
                        f->mode = NEI_PERM;
                    }
                }
            }

            switch (f->mode) {
            case NEI_ADD:
                f->mode = NEI_SWAP;
                break;
            case NEI_SWAP:
                f->mode = NEI_REMOVE;
                break;
            case NEI_REMOVE:
                f->mode = NEI_ADD;
                break;
            default:
                break;
            }
        }
    }

    return 1;
}

void nei_factory_free(NeighborFactory* f) {
    free(f->vertex_perm);
}

static void* tth_mem_iter;
TwoThreeNode* tth_request_mem() {
    void* mem = tth_mem_iter;
    tth_mem_iter += sizeof(TwoThreeNode);
    return mem;
}

void tth_free_node(TwoThreeNode* node) {
    // no op
}

/*
void print_tth(TwoThreeNode* root, int k) {
    for (int i = 0; i < k; ++i) printf("\t");
    if (!root) { printf("Empty\n"); return; }
    
    printf("%d\n", root->h);
    print_tth(root->left, k+1);
    print_tth(root->mid, k+1);
    print_tth(root->right, k+1);
}
*/

int a_star(int* start, int* goal, int n, int* result) {
    int memory_success = 0;
    comp_graph_n = n;
   
    MemoryNode* memlist = NULL;

    //AvlNode* avl_qprior = NULL; // <priority, graph node>
    TwoThreeNode* tth_q = NULL;
    AvlNode* avl_qgraph = NULL; // <graph node, priority map node>
    AvlNode* avl_g = NULL;      // <graph node, cost>
    AvlNode* avl_c = NULL;      // <graph node>
    int *degs_goal = NULL, *degs_curr = NULL;
    NeighborFactory factory;

    AvlNode* tmp_avl_alloc = malloc(sizeof(AvlNode));
    if (!tmp_avl_alloc) {
        goto FAIL;
    }
    int* tmp_graph = malloc(sizeof(int)*n*n);
    if (!tmp_graph) {
        free(tmp_avl_alloc);
        goto FAIL;
    }

    memcpy(tmp_graph, start, sizeof(int)*n*n);
    AvlNode* avl_graph_storage = avl_create_node(tmp_avl_alloc, (size_t)tmp_graph, 0); 

    tmp_avl_alloc = malloc(sizeof(AvlNode));
    if (!tmp_avl_alloc) {
        goto FAIL;
    }
    tmp_graph = malloc(sizeof(int)*n*n);
    if (!tmp_graph) {
        free(tmp_avl_alloc);
        goto FAIL;
    }
    memcpy(tmp_graph, goal, sizeof(int)*n*n);
    avl_insert(&avl_graph_storage, tmp_avl_alloc, (size_t)tmp_graph, 0, comp_graph);
    
    // calculate degs
    degs_goal = malloc(sizeof(int)*n); // 
    if (!degs_goal) {
        goto FAIL;
    }
    degs_curr = malloc(sizeof(int)*n); // 
    if (!degs_curr) {
        goto FAIL;
    }
    
    // : dry
    for (int i = 0; i < n; ++i) {
        degs_goal[i] = 0;
        for (int j = 0; j < n; ++j) {
            degs_goal[i] += goal[n*i+j];
        }
    }
    heapsort(degs_goal, sizeof(int), n, comp_int_ptr);

    // insert start
    AvlNode* start_graph_node = avl_find(avl_graph_storage, (size_t)start, comp_graph);
    AvlNode* goal_graph_node = avl_find(avl_graph_storage, (size_t)goal, comp_graph);
   
    if (!bulk_alloc(&memlist, sizeof(TwoThreeNode))) {
        goto FAIL;
    }

    tth_create_node(memlist->mem, h(start, n, degs_curr, degs_goal), (size_t)start_graph_node);
    tth_q = memlist->mem;
    TwoThreeNode* startPriorityNode = tth_q;

    tmp_avl_alloc = malloc(sizeof(AvlNode)); // 
    if (!tmp_avl_alloc) {
        goto FAIL;
    }
    avl_insert(&avl_g, tmp_avl_alloc, (size_t)start_graph_node, 0, comp_ptr);

    tmp_avl_alloc = malloc(sizeof(AvlNode));
    if (!tmp_avl_alloc) {
        goto FAIL;
    }
    avl_insert(&avl_qgraph, tmp_avl_alloc, (size_t)start_graph_node, (size_t)startPriorityNode, comp_ptr);

    while (tth_q) {
        TwoThreeNode* min_priority_node = tth_min(tth_q);
        AvlNode* current_node = (AvlNode*)min_priority_node->value;

        //printf("Height: %lu\n", tth_nr(tth_q));
        //print_tth(tth_q, 0);

        tth_delete(&tth_q, tth_free_node, min_priority_node, comp_uint);
        
        if (current_node == goal_graph_node) {
            *result = avl_find(avl_g, (size_t)current_node, comp_ptr)->value;
            goto SUCCESS;
        }
        
        tmp_avl_alloc = malloc(sizeof(AvlNode));
        if (!tmp_avl_alloc) {
            goto FAIL;
        }
        avl_insert(&avl_c, tmp_avl_alloc, (size_t)current_node, 0, comp_ptr);
        if (!nei_factory_init(&factory, (int*)current_node->key, n)) {
            goto FAIL;
        }

        int* neighbor = NULL;
        while (1) {
            int d = ((factory.mode == NEI_ADD || factory.mode == NEI_REMOVE) ? add_remove_cost : 
                    (factory.mode == NEI_SWAP ? swap_cost : permutation_cost));
            if (!next_neighbor(&factory, &neighbor)) {
                nei_factory_free(&factory);
                goto FAIL; 
            }
            if (!neighbor) {
                break;
            } 

            AvlNode* neighbor_node = avl_find(avl_graph_storage, (size_t)neighbor, comp_graph);

            if (!neighbor_node) {
                tmp_avl_alloc = malloc(sizeof(AvlNode));
                if (!tmp_avl_alloc) {
                    nei_factory_free(&factory);
                    free(neighbor);
                    goto FAIL; 
                }
                avl_insert(&avl_graph_storage, tmp_avl_alloc, (size_t)neighbor, 0, comp_graph);
                neighbor_node = tmp_avl_alloc;
            } else {
                free(neighbor);
                neighbor = (int*)neighbor_node->key;
            }

            if (avl_find(avl_c, (size_t)neighbor_node, comp_ptr)) {
                continue;
            }
    
            size_t cost = d + avl_find(avl_g, (size_t)current_node, comp_ptr)->value;
            AvlNode* g_nei_node = avl_find(avl_g, (size_t)neighbor_node, comp_ptr);
            AvlNode* qg_nei_node = avl_find(avl_qgraph, (size_t)neighbor_node, comp_ptr);
    
            if (!g_nei_node || cost < g_nei_node->value) {
                if (!g_nei_node) {
                    tmp_avl_alloc = malloc(sizeof(AvlNode));
                    if (!tmp_avl_alloc) {
                        nei_factory_free(&factory);
                        goto FAIL;
                    }
                    avl_insert(&avl_g, tmp_avl_alloc, (size_t)neighbor_node, cost, comp_ptr);
                } else {
                    g_nei_node->value = cost;
                }
                if (!qg_nei_node) {
                    TwoThreeNode* priority_node;
                    if (!bulk_alloc(&memlist, sizeof(TwoThreeNode) * ((tth_q ? tth_q->h : 0)+3))) {
                        nei_factory_free(&factory);
                        goto FAIL;
                    }
                    tth_mem_iter = memlist->mem;
                    tth_insert(&tth_q, &priority_node, cost + h(neighbor, n, degs_curr, degs_goal), (size_t)neighbor_node, tth_request_mem, comp_uint);
                    tmp_avl_alloc = malloc(sizeof(AvlNode));
                    if (!tmp_avl_alloc) {
                        nei_factory_free(&factory);
                        goto FAIL;
                    }
                    avl_insert(&avl_qgraph, tmp_avl_alloc, (size_t)neighbor_node, (size_t)priority_node, comp_ptr);
                } else {
                    tth_modify_key((TwoThreeNode*)qg_nei_node->value, cost + h(neighbor, n, degs_curr, degs_goal), comp_uint);
                }
            }
        }
        nei_factory_free(&factory);
    }
    
    *result = -1;
SUCCESS:
    memory_success = 1;
FAIL:
    bulk_free(memlist);
    free(degs_goal);
    free(degs_curr);
    avl_free(avl_c);
    avl_free(avl_g);
    avl_free(avl_qgraph);
    avl_for_each(avl_graph_storage, free_graph_storage_unit);
    avl_free(avl_graph_storage);
    return memory_success;
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
    /*
    int A[] = {
        0, 1, 0, 0, 0,
        0, 0, 0, 1, 0,
        0, 1, 0, 0, 0,
        0, 1, 0, 0, 0,
        0, 0, 0, 0, 1,
    };

    int B[] = {
        0, 0, 0, 0, 0,
        0, 0, 0, 1, 0,
        1, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 1, 0, 0, 1,
    };*/

    int A[] = {
        0, 1, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 1, 0,
        0, 100, 0, 0, 1,
        1, 0, 0, 0, 0,
    };

    int B[] = {
        0, 0, 0, 0, 1,
        1, 0, 0, 0, 0,
        0, 1, 0, 0, 0,
        1, 0, 1, 0, 0,
        0, 0, 0, 1, 0,
    };

    int result;
    int success = calculate_graph_distance(A, 5, B, 5, &result);

    if (!success) return EXIT_FAILURE;
    printf("Result: %d\n", result);
    
    return EXIT_SUCCESS;
}
