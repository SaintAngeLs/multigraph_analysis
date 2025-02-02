#include "cycle_list.h"

void initCycleList(CycleList* cl) {
    cl->count = 0;
    cl->capacity = INITIAL_CYCLELIST_CAPACITY;
    cl->cycles = (int**)malloc(cl->capacity * sizeof(int*));
    cl->sizes = (int*)malloc(cl->capacity * sizeof(int));
}

void freeCycleList(CycleList* cl) {
    if (!cl) return;
    for (int i = 0; i < cl->count; i++) {
        free(cl->cycles[i]);
    }
    free(cl->cycles);
    free(cl->sizes);
    cl->count = 0;
    cl->capacity = 0;
}

void addCycle(CycleList* cl, const int* cycle, int length) {
    if (cl->count == cl->capacity) {
        cl->capacity *= 2;
        cl->cycles = (int**)realloc(cl->cycles, cl->capacity * sizeof(int*));
        cl->sizes = (int*)realloc(cl->sizes, cl->capacity * sizeof(int));
        if (!cl->cycles || !cl->sizes) {
            fprintf(stderr, "Memory reallocation failed in addCycle.\n");
            return;
        }
    }
    int* newCycle = (int*)malloc(length * sizeof(int));
    memcpy(newCycle, cycle, length * sizeof(int));

    cl->cycles[cl->count] = newCycle;
    cl->sizes[cl->count] = length;
    cl->count++;
}