#ifndef MIME_H
#define MIME_H

void init_mime_types();
const char *get_mime_type(const char *filepath);
void free_mime_types();

#endif
