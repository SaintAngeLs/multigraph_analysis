#include "avl.h"
#include "avl_impl.h"
#include <string.h>

DataQueryKeyAvlNode *avl_insert(DataQueryKeyAvlNode *tree_root,
                                DataQueryKey elem,
                                DataQueryKeyAvlNode *allocMem,
                                int (*comp)(const void *, const void *)) {
  DataQueryKeyAvlNode fakeNode;
  memset(&fakeNode, 0, sizeof(DataQueryKeyAvlNode));
  fakeNode.value = elem;
  return avl_insert_impl(
      tree_root, &fakeNode, allocMem, sizeof(DataQueryKeyAvlNode), comp,
      offsetof(DataQueryKeyAvlNode, left), offsetof(DataQueryKeyAvlNode, right),
      offsetof(DataQueryKeyAvlNode, ht));
}

DataQueryKeyAvlNode *avl_delete_value(DataQueryKeyAvlNode *tree_root,
                                      DataQueryKey valueElem,
                                      int (*comp)(const void *, const void *)) {
  DataQueryKeyAvlNode fakeNode;
  memset(&fakeNode, 0, sizeof(DataQueryKeyAvlNode));
  fakeNode.value = valueElem;
  return avl_erase_by_value_impl(
      tree_root, &fakeNode, sizeof(DataQueryKeyAvlNode), comp,
      offsetof(DataQueryKeyAvlNode, left), offsetof(DataQueryKeyAvlNode, right),
      offsetof(DataQueryKeyAvlNode, ht));
}

DataQueryKeyAvlNode *avl_find(DataQueryKeyAvlNode *tree_root,
                              DataQueryKey valueElem,
                              int (*comp)(const void *, const void *)) {
  DataQueryKeyAvlNode fakeNode;
  memset(&fakeNode, 0, sizeof(DataQueryKeyAvlNode));
  fakeNode.value = valueElem;
  return avl_find_impl(tree_root, comp, &fakeNode,
                       offsetof(DataQueryKeyAvlNode, left),
                       offsetof(DataQueryKeyAvlNode, right));
}

DataQueryKeyAvlNode2 *avl_insert2(DataQueryKeyAvlNode2 *tree_root,
                                  DataQueryKey elem,
                                  DataQueryKeyAvlNode2 *allocMem,
                                  int (*comp)(const void *, const void *)) {
  DataQueryKeyAvlNode2 fakeNode;
  memset(&fakeNode, 0, sizeof(DataQueryKeyAvlNode2));
  fakeNode.value = elem;
  return avl_insert_impl(tree_root, &fakeNode, allocMem,
                         sizeof(DataQueryKeyAvlNode2), comp,
                         offsetof(DataQueryKeyAvlNode2, left),
                         offsetof(DataQueryKeyAvlNode2, right),
                         offsetof(DataQueryKeyAvlNode2, ht));
}

DataQueryKeyAvlNode2 *
avl_delete_value2(DataQueryKeyAvlNode2 *tree_root, DataQueryKey elem,
                  int (*comp)(const void *, const void *)) {
  DataQueryKeyAvlNode2 fakeNode;
  memset(&fakeNode, 0, sizeof(DataQueryKeyAvlNode2));
  fakeNode.value = elem;
  return avl_erase_by_value_impl(tree_root, &fakeNode,
                                 sizeof(DataQueryKeyAvlNode2), comp,
                                 offsetof(DataQueryKeyAvlNode2, left),
                                 offsetof(DataQueryKeyAvlNode2, right),
                                 offsetof(DataQueryKeyAvlNode2, ht));
}

DataQueryKeyAvlNode2 *avl_find2(DataQueryKeyAvlNode2 *tree_root,
                                DataQueryKey elem,
                                int (*comp)(const void *, const void *)) {
  DataQueryKeyAvlNode2 fakeNode;
  memset(&fakeNode, 0, sizeof(DataQueryKeyAvlNode2));
  fakeNode.value = elem;
  return avl_find_impl(tree_root, comp, &fakeNode,
                       offsetof(DataQueryKeyAvlNode2, left),
                       offsetof(DataQueryKeyAvlNode2, right));
}