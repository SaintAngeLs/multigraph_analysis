#ifndef MINIML_AVL_IMPL_H
#define MINIML_AVL_IMPL_H
#include <stddef.h>

/* elem: node */
void *avl_find_impl(void *root, int (*comp)(const void *, const void *),
                    void *elem, size_t left_offset, size_t right_offset);

/* elem with value: node */
void *avl_erase_by_value_impl(void *tree_root, void *elem, size_t node_size,
                              int (*comp)(const void *, const void *),
                              size_t loft, size_t roft, size_t hoft);

/* allocMem: prepared memory */
void *avl_insert_impl(void *tree_root, void *elem, void *allocMem,
                      size_t node_size, int (*comp)(const void *, const void *),
                      size_t loft, size_t roft, size_t hoft);

#endif