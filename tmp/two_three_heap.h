#ifndef TWO_THREE_HEAP_H
#define TWO_THREE_HEAP_H
#include <stddef.h>
#include <string.h>
#include <assert.h>

typedef struct TwoThreeNode_ {
    // the leaf value or the pointer to the min node
    size_t key;
    
    size_t value;
    struct TwoThreeNode_ *left, *mid, *right, *parent;
    int_fast32_t h;
} TwoThreeNode;

int tth_is_leaf(TwoThreeNode* node);

int tth_2(TwoThreeNode* node);

int tth_3(TwoThreeNode* node);

size_t tth_get_key(TwoThreeNode* node);

size_t tth_get_key_node(TwoThreeNode* node);

TwoThreeNode* tth_get_rightest(TwoThreeNode* node);

TwoThreeNode** tth_empty_slot(TwoThreeNode* node);

TwoThreeNode** tth_get_parent_handle(TwoThreeNode* node);

void tth_update_min(TwoThreeNode* node, int (*comp)(const void*, const void*));

TwoThreeNode* tth_get_sibling(TwoThreeNode* node);


TwoThreeNode* tth_min(TwoThreeNode* root);

void tth_create_node(TwoThreeNode* mem, size_t key, size_t value);

int tth_insert(TwoThreeNode** root_ptr, TwoThreeNode** out, size_t key, size_t value, TwoThreeNode* (*request_mem)(), int (*comp)(const void*, const void*));

int tth_union(TwoThreeNode** dst_ptr, TwoThreeNode* src, TwoThreeNode* (*request_mem)(), int (*comp)(const void*, const void*));

void tth_delete(TwoThreeNode** root_ptr, void (*free_node)(TwoThreeNode*), TwoThreeNode* target, int (*comp)(const void*, const void*));

void tth_modify_key(TwoThreeNode* target, size_t key, int (*comp)(const void*, const void*));

#endif
