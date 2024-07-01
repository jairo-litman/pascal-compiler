#include "hash.h"

#include <stdint.h>
#include <string.h>

// Hash function for strings
uint32_t hStrHash(const void *key) {
    const char *str  = (const char *)key;
    uint32_t    hash = 0;
    while (*str) {
        hash = (hash * 31) + *str++;
    }
    return hash;
}

// Compare function for strings
int32_t hStrCmp(const void *a, const void *b) { return strcmp((const char *)a, (const char *)b); }

// Hash function for 32 bit signed integers
uint32_t hI32Hash(const void *key) {
    const int32_t *i32  = (const int32_t *)key;
    uint32_t       hash = (uint32_t)*i32;
    hash                = ((hash >> 16) ^ hash) * 0x45d9f3b;
    hash                = ((hash >> 16) ^ hash) * 0x45d9f3b;
    hash                = (hash >> 16) ^ hash;
    return hash;
}

// Compare function for 32 bit signed integers
int32_t hI32Cmp(const void *a, const void *b) {
    const int32_t *i32a = (const int32_t *)a;
    const int32_t *i32b = (const int32_t *)b;
    return *i32a - *i32b;
}