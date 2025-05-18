#ifndef _IO_H_DEFINED
#define _IO_H_DEFINED

#include <sys/types.h>

typedef __time_t time_t;
typedef u_int32_t _fsize_t;

// #define FINDDATA struct _finddata_t;


#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>

#define _A_NORMAL 0x00
#define _A_SUBDIR 0x10

struct _finddata_t {
    unsigned attrib;
    time_t time_write;
    size_t size;
    char name[260];  // or PATH_MAX
};

typedef struct {
    DIR *dir;
    char pattern[260];
    char path[260];
} _find_handle_t;

intptr_t _findfirst(const char *pattern, struct _finddata_t *data) {
    _find_handle_t *handle = malloc(sizeof(_find_handle_t));
    if (!handle) return -1;

    // Split path from pattern (e.g. "dir/*.c")
    const char *slash = strrchr(pattern, '/');
    if (slash) {
        size_t len = slash - pattern + 1;
        strncpy(handle->path, pattern, len);
        handle->path[len] = '\0';
        strncpy(handle->pattern, slash + 1, sizeof(handle->pattern));
    } else {
        strcpy(handle->path, "./");
        strncpy(handle->pattern, pattern, sizeof(handle->pattern));
    }

    handle->dir = opendir(handle->path);
    if (!handle->dir) {
        free(handle);
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(handle->dir)) != NULL) {
        if (fnmatch(handle->pattern, entry->d_name, 0) == 0) {
            struct stat st;
            char fullpath[512];
            snprintf(fullpath, sizeof(fullpath), "%s%s", handle->path, entry->d_name);
            if (stat(fullpath, &st) == 0) {
                strncpy(data->name, entry->d_name, sizeof(data->name));
                data->time_write = st.st_mtime;
                data->size = st.st_size;
                data->attrib = S_ISDIR(st.st_mode) ? _A_SUBDIR : _A_NORMAL;
                return (intptr_t)handle;
            }
        }
    }

    closedir(handle->dir);
    free(handle);
    return -1;
}

int _findnext(intptr_t h, struct _finddata_t *data) {
    _find_handle_t *handle = (_find_handle_t *)h;
    struct dirent *entry;

    while ((entry = readdir(handle->dir)) != NULL) {
        if (fnmatch(handle->pattern, entry->d_name, 0) == 0) {
            struct stat st;
            char fullpath[512];
            snprintf(fullpath, sizeof(fullpath), "%s%s", handle->path, entry->d_name);
            if (stat(fullpath, &st) == 0) {
                strncpy(data->name, entry->d_name, sizeof(data->name));
                data->time_write = st.st_mtime;
                data->size = st.st_size;
                data->attrib = S_ISDIR(st.st_mode) ? _A_SUBDIR : _A_NORMAL;
                return 0;
            }
        }
    }

    return -1;
}

int _findclose(intptr_t h) {
    _find_handle_t *handle = (_find_handle_t *)h;
    if (handle) {
        if (handle->dir) closedir(handle->dir);
        free(handle);
        return 0;
    }
    return -1;
}

 

#endif