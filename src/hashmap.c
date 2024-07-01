#include "hashmap.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Create a new hashmap
HashMap *hmNew(uint32_t (*hash)(const void *key), int32_t (*cmp)(const void *a, const void *b), size_t capacity) {
    HashMap *hm = malloc(sizeof(HashMap));
    if (!hm) {
        return NULL;
    }

    hm->hash     = hash;
    hm->cmp      = cmp;
    hm->capacity = capacity;

    hm->pairs = calloc(capacity, sizeof(HashPair *));
    if (!hm->pairs) {
        free(hm);
        return NULL;
    }

    return hm;
}

// Free the hashmap
void hmFree(HashMap *hm) {
    for (size_t i = 0; i < hm->capacity; i++) {
        HashPair *pair = hm->pairs[i];
        while (pair) {
            HashPair *next = pair->next;
            hmFreeHashPair(pair);
            pair = next;
        }
    }
    free(hm->pairs);
    free(hm);
}

// Free a hash pair
void hmFreeHashPair(HashPair *pair) {
    if (pair->freeKey) {
        pair->freeKey(pair->key);
    }
    if (pair->freeValue) {
        pair->freeValue(pair->value);
    }
    free(pair);
}

// Get a value from the hashmap
HashMapResult hmGet(HashMap *hm, void *key) {
    uint32_t  index = hm->hash(key) % hm->capacity;
    HashPair *pair  = hm->pairs[index];

    while (pair) {
        if (hm->cmp(pair->key, key) == 0) {
            return (HashMapResult){pair->value, true};
        }
        pair = pair->next;
    }

    return (HashMapResult){NULL, false};
}

// Insert a key-value pair into the hashmap
bool hmInsert(HashMap *hm, void *key, void *value, void (*freeKey)(void *key), void (*freeValue)(void *value)) {
    uint32_t  index = hm->hash(key) % hm->capacity;
    HashPair *pair  = hm->pairs[index];

    while (pair) {
        if (hm->cmp(pair->key, key) == 0) {
            return false;
        }
        pair = pair->next;
    }

    HashPair *newPair = malloc(sizeof(HashPair));
    if (!newPair) {
        return false;
    }

    newPair->key       = key;
    newPair->value     = value;
    newPair->freeKey   = freeKey;
    newPair->freeValue = freeValue;
    newPair->next      = hm->pairs[index];
    hm->pairs[index]   = newPair;

    return true;
}