#ifndef AVL_H
#define AVL_H
#include "data_query.h"
#include <stddef.h>
#include <stdint.h>

typedef struct AvlNode_ {
    size_t key;
    size_t value;

    struct AvlNode_ *left;
    struct AvlNode_ *right;
    size_t ht;

} AvlNode;

AvlNode *avl_insert(AvlNode *tree_root,
                                size_t elem, size_t second_value,
                                AvlNode *allocMem,
                                int (*comp)(const void *, const void *));

AvlNode *avl_delete(AvlNode *tree_root,
                                      size_t key,
                                      int (*comp)(const void *, const void *));

AvlNode *avl_find(AvlNode *tree_root,
                              size_t key,
                              int (*comp)(const void *, const void *));




/* Preparing AVL trees for each type */

/* AVL of data query keys */
/*typedef struct AvlNode_ {
  DataQueryKey value;
  size_t nr;
  struct AvlNode_ *parent_optional;

  struct AvlNode_ *left;
  struct AvlNode_ *right;
  size_t ht;

} AvlNode;

*/

/* AVL of AVLs */

/*
typedef struct AvlNode2_ {
  DataQueryKey value;
  AvlNode *subavl;
  size_t nr;
  struct AvlNode2_ *parent_optional;

  struct AvlNode2_ *left;
  struct AvlNode2_ *right;
  size_t ht;

} AvlNode2;

AvlNode *avl_insert(AvlNode *tree_root,
                                DataQueryKey elem,
                                AvlNode *allocMem,
                                int (*comp)(const void *, const void *));

AvlNode *avl_delete_value(AvlNode *tree_root,
                                      DataQueryKey valueElem,
                                      int (*comp)(const void *, const void *));

AvlNode *avl_find(AvlNode *tree_root,
                              DataQueryKey valueElem,
                              int (*comp)(const void *, const void *));

AvlNode2 *avl_insert2(AvlNode2 *tree_root,
                                  DataQueryKey elem,
                                  AvlNode2 *allocMem,
                                  int (*comp)(const void *, const void *));

AvlNode2 *
avl_delete_value2(AvlNode2 *tree_root, DataQueryKey elem,
                  int (*comp)(const void *, const void *));

AvlNode2 *avl_find2(AvlNode2 *tree_root,
                                DataQueryKey elem,
                                int (*comp)(const void *, const void *));
*/

#endif
