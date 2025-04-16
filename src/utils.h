/* utils.h */

#ifndef UTILS_H
#define UTILS_H


#include <stdio.h>
#include <stdlib.h>

#define BUFFER_CHUNK 4096

typedef struct {
    size_t bytes_read;
    char *content;
} File;

File *read_file_contents(const char *filepath);
// char *read_file_contents(const char *filepath);

#endif
