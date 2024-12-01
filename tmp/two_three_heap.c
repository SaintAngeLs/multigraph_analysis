#include "two_three_heap.h"
#include <stdio.h>

int tth_is_leaf(TwoThreeNode* node) {
    int result = node->left || node->mid || node->right;
    return !result;
}

int tth_has_2(TwoThreeNode* node) {
    int left = !!node->left;
    int mid = !!node->mid;
    int right = !!node->right;
    if (!left) {
        return mid && right;
    }
    return mid ^ right;
}

int tth_has_3(TwoThreeNode* node) {
    return node->left && node->mid && node->right;
}

void tth_create_node(TwoThreeNode* mem, size_t key, size_t value) {
    memset(mem, 0, sizeof(TwoThreeNode));
    mem->key = key;
    mem->value = value;
    mem->h = 0;
}

size_t tth_get_key(TwoThreeNode* node) {
    return (node->h ? ((TwoThreeNode*)node->key)->key : node->key);
}

size_t tth_get_key_node(TwoThreeNode* node) {
    return (node->h ? ((TwoThreeNode*)node->key) : node);
}

TwoThreeNode* tth_get_rightest(TwoThreeNode* node) {
    assert(node);
    assert(node->left || node->mid || node->right);
    return (node->right ? node->right : (node->mid ? node->mid : node->left));
}

TwoThreeNode** tth_empty_slot(TwoThreeNode* node) {
    return (!node->right ? &node->right : (!node->mid ? &node->mid : (!node->left ? &node->left : NULL)));
}

TwoThreeNode** tth_get_parent_handle(TwoThreeNode* node) {
    if (!node->parent) {
        return NULL;
    }

    if (node->parent->left == node) {
        return &node->parent->left;
    }

    if (node->parent->mid == node) {
        return &node->parent->mid;
    }

    if (node->parent->right == node) {
        return &node->parent->right;
    }

    assert(0);
    return NULL;
}

size_t tth_nr(TwoThreeNode* root) {
    if (!root) return 0;
    if (root->h == 0) return 1;
    return tth_nr(root->left) + tth_nr(root->mid) + tth_nr(root->right);
}

void tth_update_min(TwoThreeNode* node, int (*comp)(size_t, size_t)) {
    if (node->left) {
        TwoThreeNode* min_node = tth_get_key_node(node->left);
        if (node->mid && comp(tth_get_key(min_node), tth_get_key(node->mid)) > 0) min_node = tth_get_key_node(node->mid);
        if (node->right && comp(tth_get_key(min_node), tth_get_key(node->right)) > 0) min_node = tth_get_key_node(node->right);
        node->key = (size_t)min_node;
        return;
    }
    if (node->mid) {
        TwoThreeNode* min_node = tth_get_key_node(node->mid);
        if (node->right && comp(tth_get_key(min_node), tth_get_key(node->right)) > 0) min_node = tth_get_key_node(node->right);
        node->key = (size_t)min_node;
        return;
    }
    if (node->right) {
        node->key = (size_t)tth_get_key_node(node->right);
    }
}

void tth_insert(TwoThreeNode** root_ptr, TwoThreeNode** out, size_t key, size_t value, TwoThreeNode* (*request_mem)(), int (*comp)(size_t, size_t)) {
    TwoThreeNode* mem = request_mem();
    tth_create_node(mem, key, value);
    *out = mem;
    tth_union(root_ptr, mem, request_mem, comp);
}

void tth_union(TwoThreeNode** dst_ptr, TwoThreeNode* src, TwoThreeNode* (*request_mem)(), int (*comp)(size_t, size_t)) {
    if (!*dst_ptr) {
        *dst_ptr = src;
        return;
    }

    TwoThreeNode* dst = *dst_ptr;

    if (dst->h < src->h) {
        TwoThreeNode* tmp = dst;
        dst = src;
        src = tmp;
    }

    if (dst->h != src->h) {
        TwoThreeNode* target = dst; 
        
        while (target->h != src->h + 1) {
            target = tth_get_rightest(target);
        }

        do {
            TwoThreeNode** slot_ptr = tth_empty_slot(target);
            if (slot_ptr) {
                *slot_ptr = src;
                src->parent = target;

                do {
                    tth_update_min(target, comp);
                    target = target->parent;
                } while (target);
        
                *dst_ptr = dst;
                return;
            }

            TwoThreeNode* node_above = request_mem();
            memset(node_above, 0, sizeof(TwoThreeNode));
            node_above->left = target->right;
            node_above->left->parent = node_above;
            target->right = NULL;
            tth_update_min(target, comp);
            node_above->mid = src;
            node_above->h = src->h + 1;
            node_above->mid->parent = node_above;
            tth_update_min(node_above, comp);

            src = src->parent;
            target = target->parent;

        } while (target);
    }

    if (dst->h == src->h) {
        TwoThreeNode* mem = request_mem();
        memset(mem, 0, sizeof(TwoThreeNode));
        mem->h = dst->h + 1;
        mem->left = dst;
        mem->mid = src;
        src->parent = mem;
        dst->parent = mem;
        tth_update_min(mem, comp);
        dst = mem;
    }

    *dst_ptr = dst;
    return;
}

TwoThreeNode* tth_min(TwoThreeNode* root) {
    if (!root) return NULL;
    while (!tth_is_leaf(root)) {
        root = (TwoThreeNode*)root->key;
    }
    return root;
}

TwoThreeNode* tth_get_sibling(TwoThreeNode* node) {
    return ((node->parent->left && node->parent->left != node) ? node->parent->left : 
            ((node->parent->mid && node->parent->mid != node) ? node->parent->mid : 
             (node->parent->right && node->parent->right != node) ? node->parent->right : NULL));
}

void tth_delete(TwoThreeNode** root_ptr, void (*free_node)(TwoThreeNode*), TwoThreeNode* node_to_delete, int (*comp)(size_t, size_t)) {
    TwoThreeNode* root = *root_ptr;

    TwoThreeNode* target = node_to_delete->parent;
    assert(!target || target->h > 0);

    if (target) *tth_get_parent_handle(node_to_delete) = NULL;
    else {
        free_node(node_to_delete);
        *root_ptr = NULL;
        return;
    }

    free_node(node_to_delete);
    

    if (tth_has_2(target)) {
        TwoThreeNode* prev = target;
        while (target) {
            tth_update_min(target, comp);
            prev = target;
            target = target->parent;
        }
        *root_ptr = prev;
        return;
    }

    while (target->parent) {
        TwoThreeNode* sibling = tth_get_sibling(target);
        if (tth_has_2(sibling)) {
            // has 2
            TwoThreeNode* first = tth_get_rightest(sibling);
            *tth_get_parent_handle(first) = NULL;
            TwoThreeNode* second = tth_get_rightest(sibling);
            first->parent = target;
            second->parent = target;
            *tth_empty_slot(target) = first;
            *tth_empty_slot(target) = second;
            tth_update_min(target, comp);
            *tth_get_parent_handle(sibling) = NULL;
            free_node(sibling);
        } else {
            // has 3
            TwoThreeNode* moved = tth_get_rightest(sibling);
            *tth_get_parent_handle(moved) = NULL;
            tth_update_min(sibling, comp);
            *tth_empty_slot(target) = moved;
            moved->parent = target;
            tth_update_min(target, comp);
        }

        target = target->parent;
        if (tth_has_2(target) || tth_has_3(target)) {
            TwoThreeNode* prev = target;
            while (target) {
                tth_update_min(target, comp);
                prev = target;
                target = target->parent;
            }
            *root_ptr = prev;
            return;
        }
    }

    TwoThreeNode* child = tth_get_rightest(target);
    child->parent = NULL;
    free_node(target);
    *root_ptr = child;
    return;
}

void tth_modify_key(TwoThreeNode* target, size_t key, int (*comp)(size_t, size_t)) {
    target->key = key;
    target = target->parent;
    while (target) {
        tth_update_min(target, comp);
        target = target->parent;
    }
}
