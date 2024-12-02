#include <stdio.h>
#include <assert.h>
#include "avl.h"
#include "heap_sort.h"

int comp(size_t a, size_t b) {
    return (a < b) ? -1 : ((a > b) ? 1 : 0);
}

typedef struct Node_ {
    struct Node_* prev;
    struct Node_* next;
    size_t data;
} ListNode;

typedef struct Vector_ {
    void* data;
    size_t size;
} Vector;

int il_push(ListNode** top, size_t data) {
    ListNode* p = malloc(sizeof(ListNode));
    if (!p) return 0;
    p->data = data;
    p->next = NULL;
    p->prev = *top;
    p->prev->next = p;
    *top = p;
    return 1;
}

void il_pop(ListNode** top) {
    ListNode* prev = (*top)->prev;
    free(*top);
    *top = prev;
}

void il_free(ListNode* top) {
    while (top) {
        ListNode* prev = top->prev;
        free(top);
        top = prev;
    }
}

typedef struct StrongConnectedComponents_ {
    int* adjListOriginal;
    int* adjList;
    int* visited;
    ListNode* stack;
    int* lowlink;
    int* number;
    int sccCounter;
    Vector* currentSCCs;
    size_t currentSCCs_n;

} StrongConnectedComponenets;

void vector_bulk_free(Vector* arr, int n) {
    for (int i = 0; i < n; ++i) {
        free(arr[i].data);
    }
}

void sccs_init(StrongConnectedComponents* s, int* adjList) {
    memset(s, 0, sizeof(StrongConnectedComponents));
    s->adjList = adjList;
    s->sccCounter = 0;
}

// TODO

int getAdjacencyList(StrongConnectedComponents* s, int n, int node, int** adjList_ptr, int* lowestNodeId_ptr) {
    memset(visited, 0, sizeof(int)*n);
    memset(lowlink, 0, sizeof(int)*n);
    memset(number, 0, sizeof(int)*n);
    vector_bulk_free(currentSCCs, currentSCCs_n);
    memset(currentSCCs, 0, sizeof(int)*n);
    
    makeAdjListSubgraph(s, node);
    
    for (int i = node; i < n; ++i) {
        if (!visited[i]) {
            getStrongConnectedComponents(s, i);
            int* nodes;
            int nodes_size;
            getLowestIdComponent(s, &nodes, &nodes_size);
            if (nodes_size && exists(node, nodes) && exists(node+1, nodes)) {
                return getAdjacencyList(s, n, node+1, adjList_ptr, lowestNodeId_ptr);
            } else {
                int* adjList1 = nodes; // TODO
                if (!adjList1) {
                    for (int j = 0; j < n; ++j) {
                        
                    }
                }
            }
        }
    }

    *adjList_ptr = NULL;
    return 1;
}
