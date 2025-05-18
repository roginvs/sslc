#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>


#ifndef _WINDOWS_H_DEFINED
#define _WINDOWS_H_DEFINED



int strcpy_s(char* dest, size_t destsz, const char* src) {
    if (!dest || !src || destsz == 0) return EINVAL;
    size_t len = strlen(src);
    if (len >= destsz) {
        dest[0] = '\0';
        return ERANGE;
    }
    strcpy(dest, src);
    return 0;
}

int strcat_s(char *dest, size_t destsz, const char *src) {
    if (!dest || !src || destsz == 0) return EINVAL;

    size_t dest_len = strlen(dest);
    size_t src_len = strlen(src);

    if (dest_len + src_len + 1 > destsz) {
        dest[0] = '\0';
        return ERANGE;
    }

    strcat(dest, src);
    return 0;
}

int rand_s(unsigned int *randomValue) {
    if (!randomValue) return EINVAL;
    *randomValue = (unsigned int)rand();
    return 0;
}

#define GetCurrentProcessId() ((unsigned int)getpid())

#define _stricmp strcasecmp

static inline int sprintf_s(char *buffer, size_t sizeOfBuffer, const char *format, ...) {
    if (!buffer || sizeOfBuffer == 0 || !format) return EINVAL;

    va_list args;
    va_start(args, format);
    int written = vsnprintf(buffer, sizeOfBuffer, format, args);
    va_end(args);

    if (written < 0 || (size_t)written >= sizeOfBuffer) {
        buffer[0] = '\0'; // mimic Windows behavior
        return ERANGE;
    }

    return 0;
}

#endif