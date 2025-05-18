#include <string.h>

#include <errno.h>
#include <stdlib.h>

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

#include <unistd.h>
#define GetCurrentProcessId() ((unsigned int)getpid())

#endif