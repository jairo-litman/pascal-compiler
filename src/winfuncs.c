#include "winfuncs.h"

#include <string.h>

// https://github.com/msys2/MINGW-packages/issues/4999#issuecomment-1530791650
char *strndup(const char *src, size_t size) {
    size_t len = strnlen(src, size);
    len        = len < size ? len : size;
    char *dst  = malloc(len + 1);
    if (!dst) return NULL;
    memcpy(dst, src, len);
    dst[len] = '\0';
    return dst;
}
