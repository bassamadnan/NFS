#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAP_SIZE 100

struct KeyValue {
    char* key;
    void* value;
    struct KeyValue* left;
    struct KeyValue* right;
    int height;
};

struct HashMap {
    struct KeyValue* table[MAP_SIZE];
};

unsigned int hash(const char* key);
int max(int a, int b);
int height(struct KeyValue* node);
struct KeyValue* createNode(const char* key, void* value);
struct KeyValue* rightRotate(struct KeyValue* y);
struct KeyValue* leftRotate(struct KeyValue* x);
int getBalance(struct KeyValue* node);
struct KeyValue* insertNode(struct KeyValue* root, const char* key, void* value);
struct KeyValue* search(struct KeyValue* root, const char* key);
void freeTree(struct KeyValue* root);


unsigned int hash(const char* key) {
    unsigned int hash = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        hash = 31 * hash + key[i];
    }
    return hash % MAP_SIZE;
}

void initHashMap(struct HashMap* map) {
    for (int i = 0; i < MAP_SIZE; i++) {
        map->table[i] = NULL;
    }
}

void insert(struct HashMap* map, const char* key, void* value) {
    unsigned int index = hash(key);
    map->table[index] = insertNode(map->table[index], key, value);
}

void* find(struct HashMap* map, const char* key) {
    unsigned int index = hash(key);
    struct KeyValue* pair = search(map->table[index], key);
    if (pair == NULL) {
        return NULL;
    }
    return pair->value;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int height(struct KeyValue* node) {
    if (node == NULL) {
        return 0;
    }
    return node->height;
}

struct KeyValue* createNode(const char* key, void* value) {
    struct KeyValue* newNode = (struct KeyValue*)malloc(sizeof(struct KeyValue));
    if (newNode) {
        newNode->key = strdup(key);
        newNode->value = value;
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->height = 1;
    }
    return newNode;
}

struct KeyValue* rightRotate(struct KeyValue* y) {
    struct KeyValue* x = y->left;
    struct KeyValue* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;
    return x;
}

struct KeyValue* leftRotate(struct KeyValue* x) {
    struct KeyValue* y = x->right;
    struct KeyValue* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;
    return y;
}

int getBalance(struct KeyValue* node) {
    if (node == NULL) {
        return 0;
    }
    return height(node->left) - height(node->right);
}

struct KeyValue* insertNode(struct KeyValue* root, const char* key, void* value) {
    if (root == NULL) {
        return createNode(key, value);
    }
    if (strcmp(key, root->key) < 0) {
        root->left = insertNode(root->left, key, value);
    } else if (strcmp(key, root->key) > 0) {
        root->right = insertNode(root->right, key, value);
    } else {
        root->value = value;
        return root;
    }
    root->height = 1 + max(height(root->left), height(root->right));
    int balance = getBalance(root);
    if (balance > 1) {
        if (strcmp(key, root->left->key) < 0) {
            return rightRotate(root);
        } else {
            root->left = leftRotate(root->left);
            return rightRotate(root);
        }
    }
    if (balance < -1) {
        if (strcmp(key, root->right->key) > 0) {
            return leftRotate(root);
        } else {
            root->right = rightRotate(root->right);
            return leftRotate(root);
        }
    }
    return root;
}

struct KeyValue* search(struct KeyValue* root, const char* key) {
    if (root == NULL || strcmp(key, root->key) == 0) {
        return root;
    }
    if (strcmp(key, root->key) < 0) {
        return search(root->left, key);
    } else {
        return search(root->right, key);
    }
}

void freeTree(struct KeyValue* root) {
    if (root == NULL) {
        return;
    }
    freeTree(root->left);
    freeTree(root->right);
    free(root->key);
    free(root);
}

void freeHashMap(struct HashMap* map) {
    for (int i = 0; i < MAP_SIZE; i++) {
        freeTree(map->table[i]);
    }
}

