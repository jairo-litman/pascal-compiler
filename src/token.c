#include "token.h"

#include <stdlib.h>

// Create a new keyword hashmap
void initKeywords(HashMap *keywords) {
    hmInsert(keywords, "true", TRUE);
    hmInsert(keywords, "false", FALSE);
    hmInsert(keywords, "character", CHARACTER);
    hmInsert(keywords, "boolean", BOOLEAN);
    hmInsert(keywords, "integer", INTEGER);
    hmInsert(keywords, "real", REAL);
    hmInsert(keywords, "string", STRING);
    hmInsert(keywords, "mod", MOD);
    hmInsert(keywords, "div", DIV);
    hmInsert(keywords, "and", AND);
    hmInsert(keywords, "or", OR);
    hmInsert(keywords, "not", NOT);
    hmInsert(keywords, "program", PROGRAM);
    hmInsert(keywords, "begin", BEGIN);
    hmInsert(keywords, "end", END);
    hmInsert(keywords, "if", IF);
    hmInsert(keywords, "then", THEN);
    hmInsert(keywords, "else", ELSE);
    hmInsert(keywords, "continue", CONTINUE);
    hmInsert(keywords, "break", BREAK);
    hmInsert(keywords, "return", RETURN);
    hmInsert(keywords, "while", WHILE);
    hmInsert(keywords, "do", DO);
    hmInsert(keywords, "repeat", REPEAT);
    hmInsert(keywords, "until", UNTIL);
    hmInsert(keywords, "for", FOR);
    hmInsert(keywords, "to", TO);
    hmInsert(keywords, "downto", DOWNTO);
    hmInsert(keywords, "case", CASE);
    hmInsert(keywords, "of", OF);
    hmInsert(keywords, "var", VAR);
    hmInsert(keywords, "const", CONST);
    hmInsert(keywords, "type", TYPE);
    hmInsert(keywords, "function", FUNCTION);
    hmInsert(keywords, "procedure", PROCEDURE);
    hmInsert(keywords, "goto", GOTO);
    hmInsert(keywords, "label", LABEL);
    hmInsert(keywords, "nil", NIL);
}

// Lookup an identifier in the keyword hashmap, if it's not a keyword it's an identifier
TokenType lookupIdent(HashMap *keywords, char *ident) {
    TokenType tok;
    if ((tok = hmGet(keywords, ident)) != -1) {
        return tok;
    }

    return IDENT;
}

// Hash function for strings
uint32_t strHash(const char *key) {
    uint32_t hash = 0;
    while (*key) {
        hash = (hash * 31) + *key++;
    }
    return hash;
}

// Create a new hashmap with a hash and compare function
HashMap *newHashMap(uint32_t (*hash)(const char *key), int32_t (*cmp)(const char *a, const char *b)) {
    HashMap *hm = malloc(sizeof(HashMap));
    if (!hm) {
        return NULL;
    }

    hm->hash = hash;
    hm->cmp  = cmp;

    hm->pairs = calloc(HM_CAPACITY, sizeof(HashPair *));
    if (!hm->pairs) {
        free(hm);
        return NULL;
    }

    return hm;
}

// Free the hashmap
void freeHashMap(HashMap *hm) {
    for (uint32_t i = 0; i < HM_CAPACITY; i++) {
        HashPair *pair = hm->pairs[i];
        while (pair) {
            HashPair *next = pair->next;
            free(pair);
            pair = next;
        }
    }
    free(hm->pairs);
    free(hm);
}

// Get a value from the hashmap
TokenType hmGet(HashMap *hm, char *key) {
    uint32_t  index = hm->hash(key) % HM_CAPACITY;
    HashPair *pair  = hm->pairs[index];

    while (pair) {
        if (hm->cmp(pair->key, key) == 0) {
            return pair->value;
        }
        pair = pair->next;
    }

    return -1;
}

// Insert a key-value pair into the hashmap
int32_t hmInsert(HashMap *hm, char *key, TokenType value) {
    uint32_t  index = hm->hash(key) % HM_CAPACITY;
    HashPair *pair  = hm->pairs[index];

    while (pair) {
        if (hm->cmp(pair->key, key) == 0) {
            return -1;
        }
        pair = pair->next;
    }

    HashPair *newPair = malloc(sizeof(HashPair));
    if (!newPair) {
        return -1;
    }
    newPair->next  = NULL;
    newPair->key   = key;
    newPair->value = value;

    if (!hm->pairs[index]) {
        hm->pairs[index] = newPair;
    } else {
        pair = hm->pairs[index];
        while (pair->next) {
            pair = pair->next;
        }
        pair->next = newPair;
    }

    return 0;
}
