#ifndef GLIB_H
#define GLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE  1
#define FALSE 0

#define g_array_index(array, type, index) \
    (*((type*)((char*)(array)->data + (index) * (array)->element_size)))

typedef struct {
    void* data;        
    size_t len;        
    size_t capacity;   
    size_t element_size; 
} GArray;

GArray* g_array_new(size_t element_size);

int g_array_resize(GArray* array);

int g_array_append(GArray* array, void* element);

void* g_array_get(GArray* array, size_t index);

void g_array_free(GArray* array);


typedef struct HashEntry {
    void* key;
    void* value;
    struct HashEntry* next;
} HashEntry;

typedef struct GHashTable {
    HashEntry** buckets;
    size_t size;
    size_t count;
    size_t(*hash_func)(void* key);
    int (*key_cmp)(void* key1, void* key2);
    void (*key_destroy)(void* key);
    void (*value_destroy)(void* value);
} GHashTable;

size_t string_hash(void* key);

int string_cmp(void* key1, void* key2);

GHashTable* g_hash_table_new(size_t(*hash_func)(void* key), int (*key_cmp)(void* key1, void* key2), size_t size);

GHashTable* g_hash_table_new_full(size_t(*hash_func)(void* key),
    int (*key_cmp)(void* key1, void* key2),
    void (*key_destroy)(void* key),
    void (*value_destroy)(void* value),
    size_t size);

int g_hash_table_insert(GHashTable* table, void* key, void* value);

void* g_hash_table_lookup(GHashTable* table, void* key);

int g_hash_table_remove(GHashTable* table, void* key);

void g_hash_table_free(GHashTable* table);

size_t g_hash_table_size(GHashTable* table);

void key_destroy(void* key);

void value_destroy(void* value);


size_t g_str_hash(void* key);

int g_str_equal(void* key1, void* key2);

#endif // GLIB_H