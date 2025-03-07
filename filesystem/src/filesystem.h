#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdlib.h>

typedef struct Path {
    const char **files;
    size_t length;
} Path;


#endif // FILESYSTEM_H
