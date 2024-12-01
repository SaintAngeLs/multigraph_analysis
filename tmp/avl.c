#include "avl.h"

int_fast32_t height(AvlNode* node) {
    if (node == NULL)
        return 0;
    return node->height;
}

AvlNode* avl_create_node(AvlNode* alloc_mem, size_t key, size_t value) {
    AvlNode* newNode = alloc_mem;
    newNode->key = key;
    newNode->value = value;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->height = 1;
    return newNode;
}

static int_fast32_t max(int_fast32_t a, int_fast32_t b) {
    return (a < b) ? b : a;
}

AvlNode* rightRotate(AvlNode* y) {
    AvlNode* x = y->left;
    AvlNode* T2 = x->right;
    
    x->right = y;
    y->left = T2;
    
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;
    
    return x;
}

AvlNode* leftRotate(AvlNode* x) {
    AvlNode* y = x->right;
    AvlNode* T2 = y->left;
    
    y->left = x;
    x->right = T2;
    
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;
    
    return y;
}

int_fast32_t getBalance(AvlNode* node) {
    if (node == NULL)
        return 0;
    return height(node->left) - height(node->right);
}

void avl_insert(AvlNode** root_ptr, AvlNode* alloc_mem, size_t key, size_t value, int (*comp)(size_t, size_t)) {
    
    if (*root_ptr == NULL) {
        *root_ptr = avl_create_node(alloc_mem, key, value);
        return;
    }

    AvlNode* root = *root_ptr;

    if (comp(key, root->key) < 0)
        avl_insert(&root->left, alloc_mem, key, value, comp);
    else if (comp(key, root->key) > 0)
        avl_insert(&root->right, alloc_mem, key, value, comp);
    else 
        return;
    
    root->height = 1 + max(height(root->left), height(root->right));

    int_fast32_t balance = getBalance(root);
    
    if (balance > 1 && comp(key, root->left->key) < 0) {
        *root_ptr = rightRotate(root);
        return;
    }
    
    if (balance < -1 && comp(key, root->right->key) > 0) {
        *root_ptr = leftRotate(root);
        return;
    }
    
    if (balance > 1 && comp(key, root->left->key) > 0) {
        root->left = leftRotate(root->left);
        *root_ptr = rightRotate(root);
        return;
    }
    
    if (balance < -1 && comp(key, root->right->key) < 0) {
        root->right = rightRotate(root->right);
        *root_ptr = leftRotate(root);
        return;
    }

    *root_ptr = root;
}

AvlNode* avl_min_node(AvlNode* node) {
    AvlNode* current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

AvlNode* avl_delete_key(AvlNode** root_ptr, size_t key, int (*comp)(size_t, size_t)) {
    if (root_ptr == NULL)
        return NULL;

    AvlNode* root = *root_ptr;
    AvlNode* deleted_node = NULL;

    if (comp(key, root->key) < 0)
        deleted_node = avl_delete_key(&root->left, key, comp);
    else if (comp(key, root->key) > 0)
        deleted_node = avl_delete_key(&root->right, key, comp);
    else {
        
        if ((root->left == NULL) || (root->right == NULL)) {
            AvlNode* temp = root->left ? root->left : root->right;

            if (temp == NULL) {
                temp = root;
                root = NULL;
            } else 
                *root = *temp; 

            deleted_node = temp;
        } else {
            
            AvlNode* temp = avl_min_node(root->right);
            root->key = temp->key;
            root->value = temp->key;
            deleted_node = avl_delete_key(&root->right, temp->key, comp);
        }
    }

    
    if (root == NULL) {
        *root_ptr = root;
        return deleted_node;
    }
    
    root->height = 1 + max(height(root->left), height(root->right));

    int_fast32_t balance = getBalance(root);
    
    if (balance > 1 && getBalance(root->left) >= 0) {
        *root_ptr = rightRotate(root);
        return deleted_node;
    }

    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        *root_ptr = rightRotate(root);
        return deleted_node;
    }
    
    if (balance < -1 && getBalance(root->right) <= 0) {
        *root_ptr = leftRotate(root);
        return deleted_node;
    }
    
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        *root_ptr = leftRotate(root);
        return deleted_node;
    }

    *root_ptr = root;
    return deleted_node;
}

AvlNode* avl_find(AvlNode* root, size_t key, int (*comp)(size_t, size_t)) {
    if (!root) {
        return NULL;
    }
    if (comp(key, root->key) == 0) {
        return root;
    }
    if (comp(key, root->key) < 0) {
        return avl_find(root->left, key, comp);
    }
    if (comp(key, root->key) > 0) {
        return avl_find(root->right, key, comp);
    }
    return NULL;
}
