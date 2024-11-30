#include "avl.h"
#include "avl_impl.h"
#include <string.h>

GraphStorageNode *avl_insert_gstor(GraphStorageNode *tree_root,
                                void* elem, void* second_value,
                                GraphStorageNode *allocMem,
                                int (*comp)(const void *, const void *)) {
  GraphStorageNode fakeNode;
  memset(&fakeNode, 0, sizeof(GraphStorageNode));
  fakeNode.value = elem;
  fakeNode.second_value = second_value;
  return avl_insert_impl(
      tree_root, &fakeNode, allocMem, sizeof(GraphStorageNode), comp,
      offsetof(GraphStorageNode, left), offsetof(GraphStorageNode, right),
      offsetof(GraphStorageNode, ht));
}

GraphStorageNode *avl_delete_value_gstor(GraphStorageNode *tree_root,
                                      void* valueElem,
                                      int (*comp)(const void *, const void *)) {
  GraphStorageNode fakeNode;
  memset(&fakeNode, 0, sizeof(GraphStorageNode));
  fakeNode.value = valueElem;
  return avl_erase_by_value_impl(
      tree_root, &fakeNode, sizeof(GraphStorageNode), comp,
      offsetof(GraphStorageNode, left), offsetof(GraphStorageNode, right),
      offsetof(GraphStorageNode, ht));
}

GraphStorageNode *avl_find_gstor(GraphStorageNode *tree_root,
                              void* valueElem,
                              int (*comp)(const void *, const void *)) {
  GraphStorageNode fakeNode;
  memset(&fakeNode, 0, sizeof(GraphStorageNode));
  fakeNode.value = valueElem;
  return avl_find_impl(tree_root, comp, &fakeNode,
                       offsetof(GraphStorageNode, left),
                       offsetof(GraphStorageNode, right));
}

/*GraphStorageNode2 *avl_insert2(GraphStorageNode2 *tree_root,
                                  DataQueryKey elem,
                                  GraphStorageNode2 *allocMem,
                                  int (*comp)(const void *, const void *)) {
  GraphStorageNode2 fakeNode;
  memset(&fakeNode, 0, sizeof(GraphStorageNode2));
  fakeNode.value = elem;
  return avl_insert_impl(tree_root, &fakeNode, allocMem,
                         sizeof(GraphStorageNode2), comp,
                         offsetof(GraphStorageNode2, left),
                         offsetof(GraphStorageNode2, right),
                         offsetof(GraphStorageNode2, ht));
}

GraphStorageNode2 *
avl_delete_value2(GraphStorageNode2 *tree_root, DataQueryKey elem,
                  int (*comp)(const void *, const void *)) {
  GraphStorageNode2 fakeNode;
  memset(&fakeNode, 0, sizeof(GraphStorageNode2));
  fakeNode.value = elem;
  return avl_erase_by_value_impl(tree_root, &fakeNode,
                                 sizeof(GraphStorageNode2), comp,
                                 offsetof(GraphStorageNode2, left),
                                 offsetof(GraphStorageNode2, right),
                                 offsetof(GraphStorageNode2, ht));
}

GraphStorageNode2 *avl_find2(GraphStorageNode2 *tree_root,
                                DataQueryKey elem,
                                int (*comp)(const void *, const void *)) {
  GraphStorageNode2 fakeNode;
  memset(&fakeNode, 0, sizeof(GraphStorageNode2));
  fakeNode.value = elem;
  return avl_find_impl(tree_root, comp, &fakeNode,
                       offsetof(GraphStorageNode2, left),
                       offsetof(GraphStorageNode2, right));
}*/
