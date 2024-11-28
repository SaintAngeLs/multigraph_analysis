#ifndef AVL_H
#define AVL_H
#include "data_query.h"
#include <stddef.h>
#include <stdint.h>

/* Preparing AVL trees for each type */

/* AVL of data query keys */
typedef struct DataQueryKeyAvlNode_ {
  DataQueryKey value;
  size_t nr;
  struct DataQueryKeyAvlNode_ *parent_optional;

  struct DataQueryKeyAvlNode_ *left;
  struct DataQueryKeyAvlNode_ *right;
  size_t ht;

} DataQueryKeyAvlNode;

/* AVL of AVLs */
typedef struct DataQueryKeyAvlNode2_ {
  DataQueryKey value;
  DataQueryKeyAvlNode *subavl;
  size_t nr;
  struct DataQueryKeyAvlNode2_ *parent_optional;

  struct DataQueryKeyAvlNode2_ *left;
  struct DataQueryKeyAvlNode2_ *right;
  size_t ht;

} DataQueryKeyAvlNode2;

DataQueryKeyAvlNode *avl_insert(DataQueryKeyAvlNode *tree_root,
                                DataQueryKey elem,
                                DataQueryKeyAvlNode *allocMem,
                                int (*comp)(const void *, const void *));

DataQueryKeyAvlNode *avl_delete_value(DataQueryKeyAvlNode *tree_root,
                                      DataQueryKey valueElem,
                                      int (*comp)(const void *, const void *));

DataQueryKeyAvlNode *avl_find(DataQueryKeyAvlNode *tree_root,
                              DataQueryKey valueElem,
                              int (*comp)(const void *, const void *));

DataQueryKeyAvlNode2 *avl_insert2(DataQueryKeyAvlNode2 *tree_root,
                                  DataQueryKey elem,
                                  DataQueryKeyAvlNode2 *allocMem,
                                  int (*comp)(const void *, const void *));

DataQueryKeyAvlNode2 *
avl_delete_value2(DataQueryKeyAvlNode2 *tree_root, DataQueryKey elem,
                  int (*comp)(const void *, const void *));

DataQueryKeyAvlNode2 *avl_find2(DataQueryKeyAvlNode2 *tree_root,
                                DataQueryKey elem,
                                int (*comp)(const void *, const void *));

#endif