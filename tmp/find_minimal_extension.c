#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct ElementaryCyclesSearch_ {
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

} ElementaryCyclesSearch;

void ecs_init(ElementaryCyclesSearch* e, int n, int* adjMatrix) {
    memset(e, 0, sizeof(ElementaryCyclesSearch));
    e->n = n;
    e->adjMatrix = adjMatrix;
}

int ecs_findCycles(ElementaryCyclesSearch* e, int v, int s, int* adjMatrix) {
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
                    e->max_cycle_width = curr_min_width;
                }

                if (found_cycle) {
                    ++e->cycle_count;
                }
            }

            f = 1;
        } else if (!e->blocked[w]) {
            if (ecs_findCycles(e, w, s, adjMatrix)) {
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
    return f;
}

void ecs_getElementaryCycles(ElementaryCyclesSearch* e) {
    e->blocked = calloc(e->n, sizeof(int));
    e->B = malloc(e->n*e->n * sizeof(int));
    e->B_n = calloc(e->n, sizeof(size_t));
    e->stack = malloc((e->n+1) * sizeof(int));
    ecs_findCycles(e, 0, 0, e->adjMatrix);
}

void ecs_free(ElementaryCyclesSearch* e) {

}

typedef struct MinExtensionSearch_ {
    int cycle_count;
    int n;
    int* blocked;
    int* B;
    size_t* B_n;
    int* stack;
    size_t stack_n;

    int* adjMatrixExtended;

    int nr_lacking;
    int min_nr_lacking;

    int max_nr_ham_cycles;

} MinExtensionSearch;

void mxs_init(MinExtensionSearch* e, int n, int* adjMatrix) {
    memset(e, 0, sizeof(MinExtensionSearch));
    e->n = n;
    e->blocked = calloc(e->n, sizeof(int));
    e->B = malloc(e->n*e->n * sizeof(int));
    e->B_n = calloc(e->n, sizeof(size_t));
    e->stack = malloc((e->n+1) * sizeof(int));
    e->adjMatrixExtended = malloc(e->n*e->n*sizeof(int));
    e->min_nr_lacking = n*n;
}

int mxs_findCycles(MinExtensionSearch* e, int v, int s, int* adjMatrix, int second_stage) {
    int n = e->n;
    int f = 0;

    int prev_stack_back = e->stack_n ? e->stack[e->stack_n-1] : 0;
    if (e->stack_n && !adjMatrix[prev_stack_back*n+v]) {
        ++e->nr_lacking;
    }

    if (e->nr_lacking > e->min_nr_lacking) {
        --e->nr_lacking;
        return 1;
    }

    e->stack[e->stack_n] = v;
    ++e->stack_n;
    e->blocked[v] = 1;

    for (int w = 0; w < n; ++w) {
        // force full cycles
        if (w == s && e->stack_n == n) {
            if (!adjMatrix[e->stack[e->stack_n-1]*n+w]) {
                ++e->nr_lacking;
            }

            if (e->nr_lacking < e->min_nr_lacking) {
                e->cycle_count = 0;
                e->min_nr_lacking = e->nr_lacking;
            }

            if (e->nr_lacking <= e->min_nr_lacking && second_stage) {
                memcpy(e->adjMatrixExtended, adjMatrix, n*n*sizeof(int)); 
                int prev = e->stack[e->stack_n-1];
                for (int* now = e->stack; now != e->stack+e->stack_n; ++now) {
                    if (!e->adjMatrixExtended[prev*n+*now]) {
                        e->adjMatrixExtended[prev*n+*now] = 1;
                    }
                    prev = *now;
                }
            
                ElementaryCyclesSearch hamSearch;
                ecs_init(&hamSearch, n, e->adjMatrixExtended);
                ecs_getElementaryCycles(&hamSearch);
                
                //printf("Ham cycle nr: %d\n", hamSearch.cycle_count);

                if (hamSearch.cycle_count >= e->max_nr_ham_cycles) {
                    if (hamSearch.cycle_count > e->max_nr_ham_cycles) {
                        e->cycle_count = 0;
                        e->max_nr_ham_cycles = hamSearch.cycle_count;
                    }
                    ++e->cycle_count;
                }

                ecs_free(&hamSearch);
            }

            if (!adjMatrix[e->stack[e->stack_n-1]*n + w]) {
                --e->nr_lacking;
            }

            f = 1;
        } else if (!e->blocked[w]) {
            if (mxs_findCycles(e, w, s, adjMatrix, second_stage)) {
                f = 1;
            }
        }
    }

    if (f) {
        unblock(e->blocked, e->B_n, e->B, e->n, v);
    } else {
        for (int w = 0; w < n; ++w) {
            if (!exists(e->B+w*n, e->B_n[w], v)) {
                e->B[w*n+e->B_n[w]] = v;
                ++e->B_n[w];
            }
        }
    }

    if (e->stack_n > 1 && !adjMatrix[prev_stack_back*n + e->stack[e->stack_n-1]]) {
        --e->nr_lacking;
    }

    //erase(e->stack, e->stack_n, v);
    --e->stack_n;
    return f;
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
    adjMatrix[6*N+5] = 1;

    MinExtensionSearch ecs;
    mxs_init(&ecs, N, adjMatrix);
    mxs_findCycles(&ecs, 0, 0, adjMatrix, 0);
    ecs.cycle_count = 0;
    memset(ecs.B_n, 0, N*sizeof(size_t));
    memset(ecs.blocked, 0, N*sizeof(int));
    fprintf(stdout, "Second stage: min nr lacking = %d\n", ecs.min_nr_lacking);
    mxs_findCycles(&ecs, 0, 0, adjMatrix, 1);
    printf("Cycle count: %d\n", ecs.cycle_count);
    printf("Max ham cycles: %d\n", ecs.max_nr_ham_cycles);

    return 0;

}



