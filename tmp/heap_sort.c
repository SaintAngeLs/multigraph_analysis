#include "heap_sort.h"

void heapify(void *arr, size_t elem_size, size_t n, size_t i,
             int (*comp)(const void *, const void *)) {
  for (;;) {
    size_t maximum, left_index, right_index;
    maximum = i;
    right_index = (i << 1) + 2;
    left_index = (i << 1) + 1;

    if (left_index < n &&
        -1 == comp(arr + maximum * elem_size, arr + left_index * elem_size))
      maximum = left_index;

    if (right_index < n &&
        1 == comp(arr + right_index * elem_size, arr + maximum * elem_size))
      maximum = right_index;

    if (maximum != i) {
      for (size_t j = 0; j < elem_size; ++j) {
        unsigned char temp = ((unsigned char *)(arr + i * elem_size))[j];
        ((unsigned char *)(arr + i * elem_size))[j] =
            ((unsigned char *)(arr + maximum * elem_size))[j];
        ((unsigned char *)(arr + maximum * elem_size))[j] = temp;
      }
      i = maximum;
    } else {
      break;
    }
  }
}

void heapify_csw(void *arr, size_t elem_size, size_t n, size_t i,
                 int (*comp)(const void *, const void *),
                 void (*swap)(void *, void *)) {
  for (;;) {
    size_t maximum, left_index, right_index;
    maximum = i;
    right_index = (i << 1) + 2;
    left_index = (i << 1) + 1;

    if (left_index < n &&
        -1 == comp(arr + maximum * elem_size, arr + left_index * elem_size))
      maximum = left_index;

    if (right_index < n &&
        1 == comp(arr + right_index * elem_size, arr + maximum * elem_size))
      maximum = right_index;

    if (maximum != i) {
      swap(arr + i * elem_size, arr + maximum * elem_size);
      i = maximum;
    } else {
      break;
    }
  }
}

void heapsort(void *arr, size_t elem_size, size_t n,
              int (*comp)(const void *, const void *)) {
  size_t i;

  for (i = (n >> 1) - 1; i != (size_t)-1; i--) {
    heapify(arr, elem_size, n, i, comp);
  }

  for (i = n - 1; i > 0; i--) {
    for (size_t j = 0; j < elem_size; ++j) {
      unsigned char temp = ((unsigned char *)(arr + i * elem_size))[j];
      ((unsigned char *)(arr + i * elem_size))[j] =
          ((unsigned char *)(arr + 0 * elem_size))[j];
      ((unsigned char *)(arr + 0 * elem_size))[j] = temp;
    }
    heapify(arr, elem_size, i, 0, comp);
  }
}

void heapsort_csw(void *arr, size_t elem_size, size_t n,
                  int (*comp)(const void *, const void *),
                  void (*swap)(void *, void *)) {
  size_t i;

  for (i = (n >> 1) - 1; i != (size_t)-1; i--) {
    heapify_csw(arr, elem_size, n, i, comp, swap);
  }

  for (i = n - 1; i > 0; i--) {
    swap(arr, arr + elem_size * i);
    heapify_csw(arr, elem_size, i, 0, comp, swap);
  }
}