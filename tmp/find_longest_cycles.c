#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "scc_tarjan.h"

int exists(int* arr, int n, int v) {
    while (n) {
        if (arr[--n] == v)
            return 1;
    }
    return 0;
}

int* find(int* arr, int v) {
    while (1) {
        if (*arr == v)
            return arr;
        ++arr;
    }
}

void erase(int* arr, int n, int v) {
    int* p = find(arr, v);
    int* last = arr+n-1;
    while (p != last) {
        *p = *(p+1);
        ++p;
    }
}

void unblock(int* blocked, size_t* B_n, int* B, int n, int node) {
    blocked[node] = 0;
    while (B_n[node] > 0) {
        int w = B[node*n];
        memmove(B+node*n, B+node*n+1, (--B_n[node])*sizeof(int));
        if (blocked[w]) {
            unblock(blocked, B_n, B, n, w);
        }
    }
}

typedef struct CycleNode_ {
    struct CycleNode_ *prev;
    int* cycle;
    int n;
} CycleNode;

void cycles_free(CycleNode* top) {
    while (top) {
        CycleNode* prev = top->prev;
        free(top->cycle);
        free(top);
        top = prev;
    }
}

int* cycles_push(CycleNode** top, int n) {
    CycleNode* p = malloc(sizeof(CycleNode));
    if (!p) return NULL;
    p->cycle = malloc(sizeof(int)*n);
    if (!p->cycle) {
        free(p);
        return NULL;
    }
    p->prev = *top;
    p->n = 0;
    *top = p;
    return p->cycle;
}

typedef struct ElementaryCyclesSearch_ {
    CycleNode* cycles;
    int cycle_count;
    int n;
    int* adjMatrix;
    int* blocked;
    int* B;
    size_t* B_n;
    int* stack;
    size_t stack_n;

    int max_cycle_size;
    int max_cycle_width;

    // scc
    int* disc;
    int* low;
    int* stackMember;
    int* stack_scc;
    int* results_scc;
    int* scc;

} ElementaryCyclesSearch;

void ecs_free(ElementaryCyclesSearch* e) {
    cycles_free(e->cycles);
    free(e->stack);
    free(e->B_n);
    free(e->B);
    free(e->blocked);
    free(e->disc);
    free(e->low);
    free(e->stackMember);
    free(e->stack_scc);
    free(e->results_scc);
    free(e->scc);
}

int ecs_init(ElementaryCyclesSearch* e, int n, int* adjMatrix) {
    memset(e, 0, sizeof(ElementaryCyclesSearch));
    e->n = n;
    e->adjMatrix = adjMatrix;

    e->blocked = calloc(e->n, sizeof(int));
    if (!e->blocked) goto FAIL;

    e->B = malloc(e->n*e->n * sizeof(int));
    if (!e->B) goto FAIL;

    e->B_n = calloc(e->n, sizeof(size_t));
    if (!e->B_n) goto FAIL;

    e->stack = malloc((e->n+1) * sizeof(int)); 
    if (!e->stack) goto FAIL;

    e->disc = malloc((e->n+3) * sizeof(int));
    if (!e->disc) goto FAIL;

    e->low = malloc((e->n+3) * sizeof(int));
    if (!e->low) goto FAIL;

    e->stackMember = malloc((e->n+3) * sizeof(int));
    if (!e->stackMember) goto FAIL;

    e->stack_scc = malloc((e->n+3) * sizeof(int));
    if (!e->stack_scc) goto FAIL;

    e->results_scc = malloc((e->n+3) * sizeof(int));
    if (!e->results_scc) goto FAIL;

    e->scc = malloc(e->n * e->n * sizeof(int));
    if (!e->scc) goto FAIL;

    return 1;

FAIL:
    ecs_free(e);
    return 0;
}

int ecs_findCycles(ElementaryCyclesSearch* e, int v, int s, int* adjMatrix, int* result) {
    int n = e->n;
    int f = 0;
    e->stack[e->stack_n] = v;
    ++e->stack_n;
    e->blocked[v] = 1;

    for (int w = 0; w < n; ++w) {
        int ww = adjMatrix[v*n+w];
        if (!ww) continue;

        if (w == s) {
            if (e->stack_n >= e->max_cycle_size) {
                if (e->stack_n > e->max_cycle_size) {
                    e->cycle_count = 0;
                    e->max_cycle_size = e->stack_n;
                    e->max_cycle_width = 0;
                }

                int found_cycle = 1;
                int prev = e->stack[e->stack_n-1];
                int curr_min_width = INT_MAX;
                for (int* now = e->stack; now != e->stack + e->stack_n; ++now) {
                    if (curr_min_width > adjMatrix[prev*n+*now])
                        curr_min_width = adjMatrix[prev*n+*now];
                    if (curr_min_width < e->max_cycle_width) {
                        found_cycle = 0;
                        break;
                    }
                    prev = *now;
                }
                
                if (curr_min_width > e->max_cycle_width) {
                    e->cycle_count = 0;
                    cycles_free(e->cycles);
                    e->cycles = NULL;
                    e->max_cycle_width = curr_min_width;
                }

                if (found_cycle) {
                    ++e->cycle_count;
                }

                if (found_cycle) {
                    if (!cycles_push(&e->cycles, e->stack_n)) return 0;
                    memcpy(e->cycles->cycle, e->stack, sizeof(int)*e->stack_n);
                    e->cycles->n = e->stack_n;
                }    
            }

            f = 1;
        } else if (!e->blocked[w]) {
            int res;
            if (!ecs_findCycles(e, w, s, adjMatrix, &res)) {
                return 0;
            }
            if (res) {
                f = 1;
            }
        }
    }

    if (f) {
        unblock(e->blocked, e->B_n, e->B, e->n, v);
    } else {
        for (int w = 0; w < n; ++w) {
            int ww = adjMatrix[v*n+w];
            if (!ww) continue;
            if (!exists(e->B+w*n, e->B_n[w], v)) {
                e->B[w*n+e->B_n[w]] = v;
                ++e->B_n[w];
            }
        }
    }

    erase(e->stack, e->stack_n, v);
    --e->stack_n;
    *result = f;
    return 1;
}

int ecs_getElementaryCycles(ElementaryCyclesSearch* e) {
    int* matrix = e->adjMatrix;
    int top = -1;
    int time = 0;
    int label = 0;
    SCC(matrix, e->n, e->disc, e->low, e->stackMember, e->stack_scc, &top, &time, e->results_scc, &label);
    
    for (int ss = 0; ss < e->n; ++ss) {
        int exists = 0;
        int s;
        memset(e->scc, 0, sizeof(int)*e->n*e->n);
        for (int k = 0; k < e->n; ++k) {
            if (e->results_scc[k] == ss) {
                if (!exists) {
                    exists = 1;
                    s = k;
                }
                for (int l = 0; l < e->n; ++l) {
                    if (e->results_scc[l] == ss) {
                        e->scc[k*e->n+l] = matrix[k*e->n+l];
                    }
                }
                e->B_n[k] = 0;
                e->blocked[k] = 0;
            }
        }
        if (exists) {
            int dummy;
            if (!ecs_findCycles(e, s, s, e->scc, &dummy)) {
                return 0;
            }
        }
    }

    return 1;
}

int main() {
    // more than 11 takes too much RAM
    int N = 8;

    int adjMatrix[64] = {0};

    adjMatrix[0*N+1] = 1;
    adjMatrix[1*N+0] = 1;
    adjMatrix[0*N+2] = 1;
    adjMatrix[2*N+0] = 1;
    adjMatrix[1*N+3] = 1;
    adjMatrix[3*N+1] = 1;
    adjMatrix[2*N+3] = 1;
    adjMatrix[3*N+2] = 1;
    adjMatrix[0*N+3] = 1;
    adjMatrix[2*N+1] = 1;
    adjMatrix[4*N+5] = 1;
    adjMatrix[5*N+4] = 1;
    adjMatrix[4*N+6] = 1;
    adjMatrix[6*N+4] = 1;
    adjMatrix[6*N+7] = 1;
    adjMatrix[7*N+6] = 1;
    adjMatrix[5*N+7] = 1;
    adjMatrix[7*N+5] = 1;
    adjMatrix[5*N+6] = 1;
    adjMatrix[7*N+4] = 1;

    adjMatrix[3*N+5] = 1;
    adjMatrix[4*N+2] = 1;

    ElementaryCyclesSearch ecs;
    
    if (!ecs_init(&ecs, N, adjMatrix)) {
        return EXIT_FAILURE;
    }
    
    if (!ecs_getElementaryCycles(&ecs)) {
        return EXIT_FAILURE;
    }
    printf("Nr of longest cycles: %d\n", ecs.cycle_count);

    for (CycleNode* node = ecs.cycles; node; node = node->prev) {
        for (int i = 0; i < node->n; ++i) {
            printf("%d -> ", node->cycle[i]);
        }
        printf("\n");
    }

    ecs_free(&ecs);
    return EXIT_SUCCESS;
}
