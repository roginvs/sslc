#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef _DIRECT_H_DEFINED
#define _DIRECT_H_DEFINED


#define _mkdir(path) mkdir((path), 0777)
#define _chdir       chdir
#define _getcwd      getcwd
#define _stat stat


#endif _DIRECT_H_DEFINED
