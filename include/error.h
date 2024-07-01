#ifndef ERROR_H
#define ERROR_H

#include <stdint.h>

typedef struct {
    char   **data;
    uint32_t size;
    uint32_t capacity;
} eErrorList;

eErrorList *eNew();
void        eFree(eErrorList *e);
void        eAdd(eErrorList *e, char *error);

#endif  // ERROR_H