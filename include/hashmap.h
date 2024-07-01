#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct HashPair {
    void            *key;
    void            *value;
    struct HashPair *next;
    void (*freeKey)(void *key);
    void (*freeValue)(void *value);
} HashPair;

typedef struct {
    HashPair **pairs;
    uint32_t (*hash)(const void *key);
    int32_t (*cmp)(const void *a, const void *b);
    size_t capacity;
} HashMap;

typedef struct {
    void *data;
    bool  ok;
} HashMapResult;

HashMap *hmNew(uint32_t (*hash)(const void *key), int32_t (*cmp)(const void *a, const void *b), size_t capacity);
void     hmFree(HashMap *hm);
void     hmFreeHashPair(HashPair *pair);

HashMapResult hmGet(HashMap *hm, void *key);
bool          hmInsert(HashMap *hm, void *key, void *value, void (*freeKey)(void *key), void (*freeValue)(void *value));

#endif  // HASHMAP_H