#include "avl.h"
#include "avl_impl.h"
#include <string.h>

AvlNode *avl_insert(AvlNode *tree_root,
                                size_t elem, size_t second_value,
                                AvlNode *allocMem,
                                int (*comp)(const void *, const void *)) {
  AvlNode fakeNode;
  memset(&fakeNode, 0, sizeof(AvlNode));
  fakeNode.value = elem;
  fakeNode.second_value = second_value;
  return avl_insert_impl(
      tree_root, &fakeNode, allocMem, sizeof(AvlNode), comp,
      offsetof(AvlNode, left), offsetof(AvlNode, right),
      offsetof(AvlNode, ht));
}

AvlNode *avl_delete_value(AvlNode *tree_root,
                                      size_t key,
                                      int (*comp)(const void *, const void *)) {
  AvlNode fakeNode;
  memset(&fakeNode, 0, sizeof(AvlNode));
  fakeNode.value = valueElem;
  return avl_erase_by_value_impl(
      tree_root, &fakeNode, sizeof(AvlNode), comp,
      offsetof(AvlNode, left), offsetof(AvlNode, right),
      offsetof(AvlNode, ht));
}

AvlNode *avl_find(AvlNode *tree_root,
                              size_t key,
                              int (*comp)(const void *, const void *)) {
  AvlNode fakeNode;
  memset(&fakeNode, 0, sizeof(AvlNode));
  fakeNode.value = valueElem;
  return avl_find_impl(tree_root, comp, &fakeNode,
                       offsetof(AvlNode, left),
                       offsetof(AvlNode, right));
}

/*AvlNode2 *avl_insert2(AvlNode2 *tree_root,
                                  DataQueryKey elem,
                                  AvlNode2 *allocMem,
                                  int (*comp)(const void *, const void *)) {
  AvlNode2 fakeNode;
  memset(&fakeNode, 0, sizeof(AvlNode2));
  fakeNode.value = elem;
  return avl_insert_impl(tree_root, &fakeNode, allocMem,
                         sizeof(AvlNode2), comp,
                         offsetof(AvlNode2, left),
                         offsetof(AvlNode2, right),
                         offsetof(AvlNode2, ht));
}

AvlNode2 *
avl_delete_value2(AvlNode2 *tree_root, DataQueryKey elem,
                  int (*comp)(const void *, const void *)) {
  AvlNode2 fakeNode;
  memset(&fakeNode, 0, sizeof(AvlNode2));
  fakeNode.value = elem;
  return avl_erase_by_value_impl(tree_root, &fakeNode,
                                 sizeof(AvlNode2), comp,
                                 offsetof(AvlNode2, left),
                                 offsetof(AvlNode2, right),
                                 offsetof(AvlNode2, ht));
}

AvlNode2 *avl_find2(AvlNode2 *tree_root,
                                DataQueryKey elem,
                                int (*comp)(const void *, const void *)) {
  AvlNode2 fakeNode;
  memset(&fakeNode, 0, sizeof(AvlNode2));
  fakeNode.value = elem;
  return avl_find_impl(tree_root, comp, &fakeNode,
                       offsetof(AvlNode2, left),
                       offsetof(AvlNode2, right));
}*/
