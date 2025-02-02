#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "string_list.h"

void initStringList(StringList* sl) {
    sl->count = 0;
    sl->capacity = INITIAL_STRINGLIST_CAPACITY;
    sl->data = (char**)malloc(sl->capacity * sizeof(char*));
}

void freeStringList(StringList* sl) {
    if (!sl) return;
    for (int i = 0; i < sl->count; i++) {
        free(sl->data[i]);
    }
    free(sl->data);
    sl->count = 0;
    sl->capacity = 0;
}

bool stringListContains(const StringList* sl, const char* str) {
    for (int i = 0; i < sl->count; i++) {
        if (strcmp(sl->data[i], str) == 0) return true;
    }
    return false;
}

void addStringToList(StringList* sl, const char* str) {
    if (sl->count == sl->capacity) {
        sl->capacity *= 2;
        sl->data = (char**)realloc(sl->data, sl->capacity * sizeof(char*));
    }
    if (!sl->data) {
        fprintf(stderr, "Memory reallocation failed in addStringToList.\n");
        return;
    }
    sl->data[sl->count] = (char*)malloc(strlen(str) + 1);
    strcpy(sl->data[sl->count], str);
    sl->count++;
}
