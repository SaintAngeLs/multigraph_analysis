#ifndef HEAP_SORT_H
#define HEAP_SORT_H
#include <stddef.h>

void heapify(void *arr, size_t elem_size, size_t n, size_t i,
             int (*comp)(const void *, const void *));

void heapify_csw(void *arr, size_t elem_size, size_t n, size_t i,
                 int (*comp)(const void *, const void *),
                 void (*swap)(void *, void *));

void heapsort(void *arr, size_t elem_size, size_t n,
              int (*comp)(const void *, const void *));

void heapsort_csw(void *arr, size_t elem_size, size_t n,
                  int (*comp)(const void *, const void *),
                  void (*swap)(void *, void *));

#endif