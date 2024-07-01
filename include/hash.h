#ifndef HASH_H
#define HASH_H

#include <stdint.h>

uint32_t hStrHash(const void *key);
int32_t  hStrCmp(const void *a, const void *b);

uint32_t hI32Hash(const void *key);
int32_t  hI32Cmp(const void *a, const void *b);

#endif  // HASH_H