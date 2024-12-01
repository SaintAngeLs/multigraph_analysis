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


const int add_remove_cost = 1;
const int swap_cost = 10000;
const int permutation_cost = 0;


int approx_dist(int* A, int* B, int n, int* result, int nr_iterations) {
    int* perm = malloc(sizeof(n)*n);
    if (!perm) return 0;
    for (int i = 0; i < n; ++i) perm[i] = i;

    int min_cost = -1.f;
    do {
        int perm_cost = 0;
        for (int i = 0; i < n; ++i) {
            if (perm[i] != i) {
                perm_cost += permutation_cost;
            }
        }
        int edge_cost = 0;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                edge_cost += add_remove_cost * abs(A[perm[i]*n+perm[j]] - B[i*n+j]);
            }
        }
        int total_cost = perm_cost + edge_cost;
        if (total_cost < min_cost || min_cost == -1.f) {
            min_cost = total_cost;
        }
    } while (--nr_iterations && next_permutation(perm, perm+n, sizeof(int), comp_int_ptr));

    *result = min_cost;
    free(perm);
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
    
    if (approx_dist(A, B, n, result, 300)) {
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
