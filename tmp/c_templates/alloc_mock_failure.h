#ifndef MINIML_XALLOC_H
#define MINIML_XALLOC_H
#include <stddef.h>

extern int enable_forking_allocation;

void *malloc_mock_failure(size_t memsz, size_t line, char *file);
void *calloc_mock_failure(size_t nmemb, size_t sz, size_t line, char *file);
void *realloc_mock_failure(void *mem, size_t memsz, size_t line, char *file);

#endif
