/* utils.c */

#include "utils.h"
#include <string.h>

File *read_file_contents(const char *filepath) {
        FILE *file = fopen(filepath, "r");
        if (!file) {
                fprintf(stderr, "ERROR: Could not open file %s\n", filepath);
                return NULL;
        }

        char *buffer = NULL;
        size_t size = 0;
        size_t capacity = 0;
        char chunk[BUFFER_CHUNK];
        size_t bytes_read;

        while ((bytes_read = fread(chunk, 1, sizeof(chunk), file)) > 0) {
                if (size + bytes_read + 1 > capacity) {
                        capacity = size + bytes_read + 1 + BUFFER_CHUNK;
                        buffer = realloc(buffer, capacity);
                        if (!buffer) {
                                fclose(file);
                                return NULL;
                        }
                }
                memcpy(buffer + size, chunk, bytes_read);
                size += bytes_read;
        }
        printf(" >> BYTESREAD: %zu\n", size);

        fclose(file);

        if (buffer) {
                buffer[size] = '\0';
        } else {
                buffer = strdup("");
        }

        File *result = malloc(sizeof(File));
        result->bytes_read = size;
        result->content = strdup(buffer);
        return result;
}
