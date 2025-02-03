#ifndef CYCLE_LIST_H
#define CYCLE_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int** cycles;
    int* sizes;
    int   count;
    int   capacity;
} CycleList;

#define INITIAL_CYCLELIST_CAPACITY 16

void initCycleList(CycleList* cl);

void freeCycleList(CycleList* cl);

/* cycle is deeply copied */
void addCycle(CycleList* cl, const int* cycle, int length);

#endif // CYCLE_LIST_H
