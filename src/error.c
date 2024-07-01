#include "error.h"

#include <stdlib.h>
#include <string.h>

// Create a new error list
eErrorList *eNew() {
    eErrorList *e = malloc(sizeof(eErrorList));
    if (e == NULL) {
        return NULL;
    }

    e->data = malloc(8 * sizeof(char *));
    if (e->data == NULL) {
        free(e);
        return NULL;
    }

    e->size     = 0;
    e->capacity = 8;

    return e;
}

// Free the error list
void eFree(eErrorList *e) {
    for (uint32_t i = 0; i < e->size; i++) {
        free(e->data[i]);
    }
    free(e->data);
    free(e);
}

// Add an error to the error list
void eAdd(eErrorList *e, char *str) {
    char *error = malloc(strlen(str) + 1);
    if (error == NULL) {
        return;
    }

    strcpy(error, str);

    if (e->size >= e->capacity) {
        e->capacity *= 2;
        e->data      = realloc(e->data, e->capacity * sizeof(char *));
        if (e->data == NULL) {
            free(error);
            return;
        }
    }

    e->data[e->size] = error;
    e->size++;
}