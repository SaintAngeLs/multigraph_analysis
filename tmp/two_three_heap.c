#include "two_tree_heap.h"

int tth_is_leaf(TwoThreeNode* node) {
    int result = node->left || node->mid || node->right;
    return !result;
}

int tth_2(TwoThreeNode* node) {
    int left = !!node->left;
    int mid = !!node->mid;
    int right = !!node->right;
    if (!left) {
        return mid && right;
    }
    return mid ^ right;
}

int tth_3(TwoThreeNode* node) {
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

void tth_update_min(TwoThreeNode* node, int (*comp)(const void*, const void*)) {
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

int tth_insert(TwoThreeNode** root_ptr, TwoThreeNode** out, size_t key, size_t value, TwoThreeNode* (*request_mem)(), int (*comp)(const void*, const void*)) {
    TwoThreeNode* mem = request_mem();
    if (!mem) return 0;
    tth_create_node(mem, key, value);
    *out = mem;
    return tth_union(root_ptr, mem, request_mem, comp);
}

int tth_union(TwoThreeNode** dst_ptr, TwoThreeNode* src, TwoThreeNode* (*request_mem)(), int (*comp)(const void*, const void*)) {
    if (!*dst_ptr) {
        *dst_ptr = src;
        return 1;
    }

    if (dst->h < src->h) {
        TwoThreeNode* tmp = dst;
        dst = src;
        src = tmp;
    }

    TwoThreeNode* dst = *dst_ptr;
    TwoThreeNode* target = dst;

    if (dst->h != src->h) {
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
                return 1;
            }

            TwoThreeNode* node_above = request_mem();
            if (!node->above) {
                return 0;
            }
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
        if (!mem) return 0;
        memset(mem, 0, sizeof(TwoThreeNode));
        mem->h = dst->h + 1;
        mem->left = dst;
        mem->mid = src;
        src->parent = mem;
        dst->parent = mem;
        tth_update_min(mem, comp);
    }

    return 1;
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

void tth_delete(TwoThreeNode** root_ptr, void (*free_node)(TwoThreeNode*), TwoThreeNode* target, int (*comp)(const void*, const void*)) {
    TwoThreeNode* root = *root_ptr;
    
    TwoThreeNode* next = target->parent;
    if (next) *tth_get_parent_handle(target) = NULL;
    else {
        free_node(target);
        *root_ptr = NULL;
        return;
    }

    free_node(target);

    if (tth_2(next)) {
        TwoThreeNode* prev = next;
        while (next) {
            tth_update_min(next, comp);
            prev = next;
            next = next->parent;
        }
        *root_ptr = prev;
        return;
    }

    while (next->parent) {
        TwoThreeNode* sibling = tth_get_sibling(next);
        if (tth_2(sibling)) {
            TwoThreeNode* first = tth_get_rightest(sibling);
            *tth_get_parent_handle(first) = NULL;
            TwoThreeNode* second = tth_get_rightest(sibling);
            first->parent = next;
            second->parent = next;
            *tth_empty_slot(next) = first;
            *tth_empty_slot(next) = second;
            tth_update_min(next, comp);
            *tth_get_parent_handle(sibling) = NULL;
            free_node(sibling);
        } else {
            TwoThreeNode* moved = tth_get_rightest(sibling);
            *tth_get_parent_handle(moved) = NULL;
            tth_update_min(sibling, comp);
            *tth_empty_slot(next) = moved;
            moved->parent = next;
            tth_update_min(next, comp);
        }

        next = next->parent;
        if (tth2(next) || tth_3(next)) {
            TwoThreeNode* prev = next;
            while (next) {
                tth_update_min(next, comp);
                prev = next;
                next = next->parent;
            }
            *root_ptr = prev;
            return;
        }
    }

    TwoThreeNode* child = tth_get_rightest(next);
    child->parent = NULL;
    free_node(next);
    *root_ptr = child;
    return;
}

void tth_modify_key(TwoThreeNode* target, size_t key, int (*comp)(const void*, const void*)) {
    target->key = key;
    target = target->parent;
    while (target) {
        tth_update_min(target, comp);
        target = target->parent;
    }
}

#endif
