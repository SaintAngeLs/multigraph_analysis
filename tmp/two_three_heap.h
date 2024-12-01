#ifndef TWO_THREE_HEAP_H
#define TWO_THREE_HEAP_H
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

typedef struct TwoThreeNode_ {
    // the leaf value or the pointer to the min node
    size_t key;
    
    size_t value;
    struct TwoThreeNode_ *left, *mid, *right, *parent;
    int_fast32_t h;
} TwoThreeNode;

size_t tth_nr(TwoThreeNode* node);

int tth_is_leaf(TwoThreeNode* node);

int tth_has_2(TwoThreeNode* node);

int tth_has_3(TwoThreeNode* node);

size_t tth_get_key(TwoThreeNode* node);

size_t tth_get_key_node(TwoThreeNode* node);

TwoThreeNode* tth_get_rightest(TwoThreeNode* node);

TwoThreeNode** tth_empty_slot(TwoThreeNode* node);

TwoThreeNode** tth_get_parent_handle(TwoThreeNode* node);

void tth_update_min(TwoThreeNode* node, int (*comp)(size_t, size_t));

TwoThreeNode* tth_get_sibling(TwoThreeNode* node);


TwoThreeNode* tth_min(TwoThreeNode* root);

void tth_create_node(TwoThreeNode* mem, size_t key, size_t value);

// note below
void tth_insert(TwoThreeNode** root_ptr, TwoThreeNode** out, size_t key, size_t value, TwoThreeNode* (*request_mem)(), int (*comp)(size_t, size_t));

// make sure that request_mem always returns a non-null pointer
void tth_union(TwoThreeNode** dst_ptr, TwoThreeNode* src, TwoThreeNode* (*request_mem)(), int (*comp)(size_t, size_t));

void tth_delete(TwoThreeNode** root_ptr, void (*free_node)(TwoThreeNode*), TwoThreeNode* target, int (*comp)(size_t, size_t));

void tth_modify_key(TwoThreeNode* target, size_t key, int (*comp)(size_t, size_t));

#endif
