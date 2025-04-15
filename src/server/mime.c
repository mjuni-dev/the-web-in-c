#include "mime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 101

typedef struct mime_s {
        char *ext;
        char *type;
        struct mime_s *next;
} Mime;

static Mime *mime_table[HASH_SIZE];

unsigned int hash(const char *str) {
        unsigned int hash = 0;
        while (*str) {
                hash = (hash * 31) + *str++;
        }
        return hash % HASH_SIZE;
}

void insert_mime(const char *ext, const char *type) {
        unsigned int i = hash(ext);
        Mime *entry = malloc(sizeof(Mime));
        entry->ext = strdup(ext);
        entry->type = strdup(type);

        // chaining to handle collisions
        entry->next = mime_table[i];
        mime_table[i] = entry;
}

void init_mime_types() {
        printf(" >> Initializing MIME types...");
        insert_mime(".html", "text/html");
        insert_mime(".css", "text/css");
        insert_mime(".js", "application/javascript");
        insert_mime(".png", "image/png");
        insert_mime(".jpg", "image/jpeg");
        insert_mime(".jpeg", "image/jpeg");
        insert_mime(".gif", "image/gif");
        insert_mime(".svg", "image/svg+xml");
        insert_mime(".ico", "image/x-icon");
        insert_mime(".json", "application/json");
        insert_mime(".txt", "text/plain");
}

const char *get_mime_type(const char *filepath) {
        const char *ext = strrchr(filepath, '.');
        if (!ext) {
                return "application/octet-stream";
        }

        unsigned int i = hash(ext);
        for (Mime *entry = mime_table[i]; entry; entry = entry->next) {
                if (strcmp(entry->ext, ext) == 0) {
                        return entry->type;
                }
        }

        return "application/octet-stream";
}

void free_mime_types() {
        printf(" >> Freeing MIME types...");
        for (int i = 0; i < HASH_SIZE; i++) {
                Mime *entry = mime_table[i];
                while (entry) {
                        Mime *next = entry->next;
                        free(entry->ext);
                        free(entry->type);
                        free(entry);
                        entry = next;
                }
                mime_table[i] = NULL;
        }
}
