#include "glib.h"

GArray* g_array_new(size_t element_size) {
    GArray* array = (GArray*)malloc(sizeof(GArray));
    if (array == NULL) {
        return NULL; // Memory allocation failed
    }
    array->len = 0;
    array->capacity = 4;  // Initial capacity (you can adjust this)
    array->element_size = element_size;
    array->data = malloc(array->capacity * array->element_size);
    if (array->data == NULL) {
        free(array);
        return NULL;
    }
    return array;
}

int g_array_resize(GArray* array) {
    array->capacity *= 2;
    void* new_data = realloc(array->data, array->capacity * array->element_size);
    if (new_data == NULL) {
        return -1;
    }
    array->data = new_data;
    return 0;
}

int g_array_append(GArray* array, void* element) {
    if (array->len == array->capacity) {
        if (g_array_resize(array) == -1) {
            return -1;
        }
    }
    memcpy((char*)array->data + array->len * array->element_size, element, array->element_size);
    array->len++;
    return 0;
}

int g_array_append_vals(GArray* array, void* elements, size_t count) {
	if (array->len + count > array->capacity) {
		while (array->len + count > array->capacity) {
			if (g_array_resize(array) == -1) {
				return -1;
			}
		}
	}
	memcpy((char*)array->data + array->len * array->element_size, elements, count * array->element_size);
	array->len += count;
	return 0;
}

void* g_array_get(GArray* array, size_t index) {
    if (index >= array->len) {
        return NULL;
    }
    return (char*)array->data + index * array->element_size;
}

void g_array_free(GArray* array) {
    free(array->data);
    free(array);
}

size_t string_hash(void* key) {
    char* str = (char*)key;
    size_t hash = 0;
    while (*str) {
        hash = (hash * 31) + *str;
        str++;
    }
    return hash;
}

int string_cmp(void* key1, void* key2) {
    return strcmp((char*)key1, (char*)key2);
}

GHashTable* g_hash_table_new(size_t(*hash_func)(void* key), bool (*key_cmp)(void* key1, void* key2), size_t size) {
    GHashTable* table = malloc(sizeof(GHashTable));
    if (!table) return NULL;

    table->buckets = calloc(size, sizeof(HashEntry*));
    if (!table->buckets) {
        free(table);
        return NULL;
    }

    table->size = size;
    table->count = 0;
    table->hash_func = hash_func;
    table->key_cmp = key_cmp;

    return table;
}

GHashTable* g_hash_table_new_full(size_t(*hash_func)(void* key),
    bool (*key_cmp)(void* key1, void* key2),
    void (*key_destroy)(void* key),
    void (*value_destroy)(void* value)) {
    GHashTable* table = malloc(sizeof(GHashTable));
    if (!table) return NULL;

    if (!table->buckets) {
        free(table);
        return NULL;
    }

    table->size = 0;
    table->count = 0;
    table->hash_func = hash_func;
    table->key_cmp = key_cmp;
    table->key_destroy = key_destroy;
    table->value_destroy = value_destroy;

    return table;
}

int g_hash_table_insert(GHashTable* table, void* key, void* value) {
    size_t hash = table->hash_func(key) % table->size;
    HashEntry* entry = table->buckets[hash];

    while (entry) {
        if (table->key_cmp(entry->key, key)) {
            entry->value = value;
            return 0; 
        }
        entry = entry->next;
    }

    entry = malloc(sizeof(HashEntry));
    if (!entry) return -1;

    entry->key = key;
    entry->value = value;
    entry->next = table->buckets[hash];
    table->buckets[hash] = entry;
    table->count++;

    return 0;
}

unsigned int direct_hash(const void* ptr) {
    return (unsigned int)(uintptr_t)ptr; 
}

bool direct_equal(const void* ptr1, const void* ptr2) {
    return ptr1 == ptr2;
}

void* g_hash_table_lookup(GHashTable* table, void* key) {
    size_t hash = table->hash_func(key) % table->size;
    HashEntry* entry = table->buckets[hash];

    while (entry) {
        if (table->key_cmp(entry->key, key)) {
            return entry->value; 
        }
        entry = entry->next;
    }

    return NULL;
}

int g_hash_table_remove(GHashTable* table, void* key) {
    size_t hash = table->hash_func(key) % table->size;
    HashEntry* entry = table->buckets[hash];
    HashEntry* prev = NULL;

    while (entry) {
        if (table->key_cmp(entry->key, key)) {
            if (prev) {
                prev->next = entry->next;
            }
            else {
                table->buckets[hash] = entry->next;
            }
            free(entry);
            table->count--;
            return 0; 
        }
        prev = entry;
        entry = entry->next;
    }

    return -1;
}

void g_hash_table_free(GHashTable* table) {
    for (size_t i = 0; i < table->size; i++) {
        HashEntry* entry = table->buckets[i];
        while (entry) {
            HashEntry* next = entry->next;
            free(entry);
            entry = next;
        }
    }
    free(table->buckets);
    free(table);
}

size_t g_hash_table_size(GHashTable* table) {
    return table->count;
}

void key_destroy(void* key) {
    free(key);
}

void value_destroy(void* value) {
    free(value);
}

size_t g_str_hash(void* key) {
    char* str = (char*)key;
    size_t hash = 14695981039346656037U; // FNV-1a offset basis
    while (*str) {
        hash ^= (unsigned char)(*str);    // XOR the byte
        hash *= 1099511628211U;            // Multiply by FNV prime
        str++;
    }
    return hash;
}

int g_str_equal(void* key1, void* key2) {
    return strcmp((char*)key1, (char*)key2) == 0; // Returns true (0) if the strings are equal
}