// https://www.w3resource.com/c-programming-exercises/tree/c-tree-exercises-10.php
// 30 Nov 2024

#ifndef AVL_H
#define AVL_H
#include <stddef.h>
#include <stdint.h>

typedef struct AvlNode_ {
    size_t key;
    size_t value;
    AvlNode_* left;
    AvlNode_* right;
    int_fast32_t height; 
} AvlNode;

AvlNode* avl_create_node(AvlNode* alloc_mem, size_t key, size_t value);

void avl_insert(AvlNode** root,  AvlNode* alloc_mem, size_t key, size_t value, int (*comp)(const void*, const void*));

AvlNode* avl_min_node(AvlNode* node);

AvlNode* avl_find_node(AvlNode* root, size_t key, int (*comp)(const void*, const void*));

AvlNode* avl_delete_key(AvlNode** root, size_t key, int (*comp)(const void*, const void*));

#endif
