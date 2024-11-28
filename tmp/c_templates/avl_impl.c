#include "avl_impl.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t avl_height(const void *tree_root, size_t loft, size_t roft,
                         size_t hoft) {
  size_t lh, rh;

  if (!tree_root)
    return 0;

  if (*(void **)(tree_root + loft) == NULL)
    lh = 0;
  else
    lh = 1 + *(size_t *)(*(void **)(tree_root + loft) + hoft);

  if (*(void **)(tree_root + roft) == NULL)
    rh = 0;
  else
    rh = 1 + *(size_t *)(*(void **)(tree_root + roft) + hoft);

  if (lh > rh)
    return lh;
  return rh;
}

static void *avl_rotateright(void *x, size_t loft, size_t roft, size_t hoft) {
  void *y;
  assert(x);
  y = *(void **)(x + loft);
  assert(y);
  *(void **)(x + loft) = *(void **)(y + roft);
  *(void **)(y + roft) = x;
  *(size_t *)(x + hoft) = avl_height(x, loft, roft, hoft);
  *(size_t *)(y + hoft) = avl_height(y, loft, roft, hoft);
  return y;
}

static void *avl_rotateleft(void *x, size_t loft, size_t roft, size_t hoft) {
  void *y;
  assert(x);
  y = *(void **)(x + roft);
  assert(y);
  *(void **)(x + roft) = *(void **)(y + loft);
  *(void **)(y + loft) = x;
  *(size_t *)(x + hoft) = avl_height(x, loft, roft, hoft);
  *(size_t *)(y + hoft) = avl_height(y, loft, roft, hoft);
  return y;
}

static int_fast64_t avl_BF(void *tree_root, size_t loft, size_t roft,
                           size_t hoft) {
  int_fast64_t lh, rh;
  if (!tree_root)
    return 0;
  if (*(void **)(tree_root + loft) == NULL)
    lh = 0;
  else
    lh = 1 + *(size_t *)(*(void **)(tree_root + loft) + hoft);
  if (*(void **)(tree_root + roft) == NULL)
    rh = 0;
  else
    rh = 1 + *(size_t *)(*(void **)(tree_root + roft) + hoft);
  return (lh - rh);
}

static void *avl_RR(void *tree_root, size_t loft, size_t roft, size_t hoft) {
  tree_root = avl_rotateleft(tree_root, loft, roft, hoft);
  return tree_root;
}
static void *avl_LL(void *tree_root, size_t loft, size_t roft, size_t hoft) {
  tree_root = avl_rotateright(tree_root, loft, roft, hoft);
  return tree_root;
}
static void *avl_LR(void *tree_root, size_t loft, size_t roft, size_t hoft) {
  *(void **)(tree_root + loft) =
      avl_rotateleft(*(void **)(tree_root + loft), loft, roft, hoft);
  tree_root = avl_rotateright(tree_root, loft, roft, hoft);
  return tree_root;
}
static void *avl_RL(void *tree_root, size_t loft, size_t roft, size_t hoft) {
  *(void **)(tree_root + roft) =
      avl_rotateright(*(void **)(tree_root + roft), loft, roft, hoft);
  tree_root = avl_rotateleft(tree_root, loft, roft, hoft);
  return tree_root;
}

static void tree_assert(void *tree_root, size_t loft, size_t roft) {
  assert(tree_root);
  assert(*(void **)(tree_root + loft) != tree_root);
  assert(*(void **)(tree_root + roft) != tree_root);

  if (*(void **)(tree_root + loft))
    tree_assert(*(void **)(tree_root + loft), loft, roft);

  if (*(void **)(tree_root + roft))
    tree_assert(*(void **)(tree_root + roft), loft, roft);
}

void *avl_insert_impl(void *tree_root, void *elem, void *allocMem,
                      size_t node_size, int (*comp)(const void *, const void *),
                      size_t loft, size_t roft, size_t hoft) {

  if (tree_root == NULL) {
    tree_root = allocMem;
    memcpy(tree_root, elem, node_size);
    *(void **)(tree_root + loft) = NULL;
    *(void **)(tree_root + roft) = NULL;
  } else if (comp(elem, tree_root) > 0) {
    /* right */
    *(void **)(tree_root + roft) =
        avl_insert_impl(*(void **)(tree_root + roft), elem, allocMem, node_size,
                        comp, loft, roft, hoft);
    if (avl_BF(tree_root, loft, roft, hoft) == -2) {
      if (comp(elem, *(void **)(tree_root + roft)) > 0) {
        tree_root = avl_RR(tree_root, loft, roft, hoft);
      } else {
        tree_root = avl_RL(tree_root, loft, roft, hoft);
      }
    }
  } else {
    /* left */
    *(void **)(tree_root + loft) =
        avl_insert_impl(*(void **)(tree_root + loft), elem, allocMem, node_size,
                        comp, loft, roft, hoft);
    if (avl_BF(tree_root, loft, roft, hoft) == 2) {
      if (comp(elem, *(void **)(tree_root + loft)) < 0) {
        tree_root = avl_LL(tree_root, loft, roft, hoft);
      } else {
        tree_root = avl_LR(tree_root, loft, roft, hoft);
      }
    }
  }
  *(size_t *)(tree_root + hoft) = avl_height(tree_root, loft, roft, hoft);

  tree_assert(tree_root, loft, roft);

  return tree_root;
}

void *avl_erase_by_value_impl(void *tree_root, void *elem, size_t node_size,
                              int (*comp)(const void *, const void *),
                              size_t loft, size_t roft, size_t hoft) {
  void *p;
  if (tree_root == NULL)
    return NULL;

  if (comp(elem, tree_root) > 0) {
    /* greater than tree_root */
    *(void **)(tree_root + roft) = avl_erase_by_value_impl(
        *(void **)(tree_root + roft), elem, node_size, comp, loft, roft, hoft);
    if (avl_BF(tree_root, loft, roft, hoft) == 2) {
      if (avl_BF(*(void **)(tree_root + loft), loft, roft, hoft) >= 0) {
        tree_root = avl_LL(tree_root, loft, roft, hoft);
      } else {
        tree_root = avl_LR(tree_root, loft, roft, hoft);
      }
    }
  } else if (comp(elem, tree_root) < 0) {
    /* less than tree_root */
    *(void **)(tree_root + loft) = avl_erase_by_value_impl(
        *(void **)(tree_root + loft), elem, node_size, comp, loft, roft, hoft);
    if (avl_BF(tree_root, loft, roft, hoft) == -2) {
      if (avl_BF(*(void **)(tree_root + loft), loft, roft, hoft) <= 0) {
        tree_root = avl_RR(tree_root, loft, roft, hoft);
      } else {
        tree_root = avl_RL(tree_root, loft, roft, hoft);
      }
    }
  } else {
    if (*(void **)(tree_root + roft) == NULL)
      return *(void **)(tree_root + loft);

    p = *(void **)(tree_root + roft);
    while (*(void **)(p + loft) != NULL) {
      p = *(void **)(p + loft);
    }

    void *tmp_left = *(void **)(tree_root + loft);
    void *tmp_right = *(void **)(tree_root + roft);
    size_t tmp_ht = *(size_t *)(tree_root + hoft);
    memcpy(tree_root, p, node_size);
    *(void **)(tree_root + loft) = tmp_left;
    *(void **)(tree_root + roft) = tmp_right;
    *(size_t *)(tree_root + hoft) = tmp_ht;

    *(void **)(tree_root + roft) = avl_erase_by_value_impl(
        *(void **)(tree_root + roft), p, node_size, comp, loft, roft, hoft);
    if (avl_BF(tree_root, loft, roft, hoft) == 2) {
      if (avl_BF(*(void **)(tree_root + loft), loft, roft, hoft) >= 0) {
        tree_root = avl_LL(tree_root, loft, roft, hoft);
      } else {
        tree_root = avl_LR(tree_root, loft, roft, hoft);
      }
    }
  }

  *(size_t *)(tree_root + hoft) = avl_height(tree_root, loft, roft, hoft);
  return tree_root;
}

void *avl_find_impl(void *root, int (*comp)(const void *, const void *),
                    void *elem, size_t left_offset, size_t right_offset) {
  for (;;) {

    if (!root) {
      return NULL;
    }

    assert(elem);
    assert(root);

    if (comp(root, elem) == 0) {
      return root;
    }

    assert(elem);
    assert(root);

    if (comp(root, elem) > 0) {
      root = *(void **)(root + left_offset);
    }

    /* don't compare nulls! */
    else

        if (comp(root, elem) < 0) {
      root = *(void **)(root + right_offset);
    }
  }
}