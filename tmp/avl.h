#ifndef AVL_H
#define AVL_H
#include "data_query.h"
#include <stddef.h>
#include <stdint.h>

// TODO: basically, void* means anything
typedef struct GraphStorageNode_ {
    void* value;
    void* second_value;

    size_t nr;
    struct GraphStorageNode_ *parent_optional;
    struct GraphStorageNode_ *left;
    struct GraphStorageNode_ *right;
    size_t ht;

} GraphStorageNode;

GraphStorageNode *avl_insert_gstor(GraphStorageNode *tree_root,
                                void* elem, void* second_value,
                                GraphStorageNode *allocMem,
                                int (*comp)(const void *, const void *));

GraphStorageNode *avl_delete_value_gstor(GraphStorageNode *tree_root,
                                      void* valueElem,
                                      int (*comp)(const void *, const void *));

GraphStorageNode *avl_find_gstor(GraphStorageNode *tree_root,
                              void* valueElem,
                              int (*comp)(const void *, const void *));




/* Preparing AVL trees for each type */

/* AVL of data query keys */
/*typedef struct GraphStorageNode_ {
  DataQueryKey value;
  size_t nr;
  struct GraphStorageNode_ *parent_optional;

  struct GraphStorageNode_ *left;
  struct GraphStorageNode_ *right;
  size_t ht;

} GraphStorageNode;

*/

/* AVL of AVLs */

/*
typedef struct GraphStorageNode2_ {
  DataQueryKey value;
  GraphStorageNode *subavl;
  size_t nr;
  struct GraphStorageNode2_ *parent_optional;

  struct GraphStorageNode2_ *left;
  struct GraphStorageNode2_ *right;
  size_t ht;

} GraphStorageNode2;

GraphStorageNode *avl_insert(GraphStorageNode *tree_root,
                                DataQueryKey elem,
                                GraphStorageNode *allocMem,
                                int (*comp)(const void *, const void *));

GraphStorageNode *avl_delete_value(GraphStorageNode *tree_root,
                                      DataQueryKey valueElem,
                                      int (*comp)(const void *, const void *));

GraphStorageNode *avl_find(GraphStorageNode *tree_root,
                              DataQueryKey valueElem,
                              int (*comp)(const void *, const void *));

GraphStorageNode2 *avl_insert2(GraphStorageNode2 *tree_root,
                                  DataQueryKey elem,
                                  GraphStorageNode2 *allocMem,
                                  int (*comp)(const void *, const void *));

GraphStorageNode2 *
avl_delete_value2(GraphStorageNode2 *tree_root, DataQueryKey elem,
                  int (*comp)(const void *, const void *));

GraphStorageNode2 *avl_find2(GraphStorageNode2 *tree_root,
                                DataQueryKey elem,
                                int (*comp)(const void *, const void *));
*/

#endif
