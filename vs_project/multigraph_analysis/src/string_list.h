#ifndef STRING_LIST_H
#define STRING_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    char** data;
    int    count;
    int    capacity;
} StringList;

#define INITIAL_STRINGLIST_CAPACITY 16

void initStringList(StringList* sl);

void freeStringList(StringList* sl);

bool stringListContains(const StringList* sl, const char* str);

void addStringToList(StringList* sl, const char* str);

#endif // CYCLE_LIST_H
