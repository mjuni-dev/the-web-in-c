/* template.c */

#include "template.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_CHUNK 4096
#define MAX_INCLUDE_DEPTH 10

#define ROOT_WEB "./src/web"

typedef struct s_template_cache {
        char *key;
        char *content;
        struct s_template_cache *next;
} TemplateCache;

static TemplateCache *cache_head = NULL;

static void cache_template(const char *key, const char *content) {
        TemplateCache *entry = malloc(sizeof(TemplateCache));
        entry->key = strdup(key);
        entry->content = strdup(content);
        entry->next = cache_head;
        cache_head = entry;
}

static char *get_cached_template(const char *key) {
        TemplateCache *entry = cache_head;
        while (entry) {
                if (strcmp(entry->key, key) == 0) {
                        return strdup(entry->content);
                }
                entry = entry->next;
        }
        return NULL;
}

static char *process_includes(const char *html, int depth) {
        if (depth > MAX_INCLUDE_DEPTH) {
                fprintf(stderr, "ERROR: Maximum include depth exceeded\n");
                return strdup(html);
        }

        char *result = NULL;
        size_t result_len = 0;
        size_t result_capacity = 0;

        // Find all {{> include/path}} patterns
        const char *curr_pos = html;
        const char *include_start = strstr(curr_pos, "{{>");

        while (include_start) {
                const char *include_end = strstr(include_start, "}}");
                if (!include_end) {
                        break;
                }

                // Append text before the include tag
                size_t prefix_len = include_start - curr_pos;
                if (result_len + prefix_len + 1 > result_capacity) {
                        result_capacity = result_len + prefix_len + BUFFER_CHUNK;
                        result = realloc(result, result_capacity);
                }
                memcpy(result + result_len, curr_pos, prefix_len);
                result_len += prefix_len;

                // Extract component path
                char component_path[256] = {0};
                const char *path_start = include_start + 3;

                // Skip whitespace
                while (*path_start == ' ' && path_start < include_end) {
                        path_start++;
                }

                const char *path_end = path_start;
                while (*path_end != ' ' && *path_end != '}' && path_end < include_end) {
                        path_end++;
                }

                int path_len = path_end - path_start;
                if (path_len > 0 && path_len < 255) {
                        strncpy(component_path, path_start, path_len);
                        component_path[path_len] = '\0';

                        // Construct component file path
                        char full_path[512] = ROOT_WEB;
                        strcat(full_path, component_path);
                        if (!strstr(full_path, ".html")) {
                                strcat(full_path, ".html");
                        }

                        // Try to get from the cache first
                        char *component_content = get_cached_template(full_path);

                        // If not in cache, process and load into cache
                        if (!component_content) {
                                // char *raw_content = read_file_contents(full_path);
                                File *file = read_file_contents(full_path);
                                if (file) {
                                        component_content =
                                            process_includes(file->content, depth + 1);

                                        cache_template(full_path, component_content);
                                        free(file);
                                } else {
                                        component_content = strdup("");
                                        fprintf(stderr,
                                                "ERROR: Could not load the "
                                                "component %s\n",
                                                full_path);
                                }
                        }

                        // Append component content
                        size_t component_len = strlen(component_content);
                        if (result_len + component_len + 1 > result_capacity) {
                                result_capacity = result_len + component_len + BUFFER_CHUNK;
                                result = realloc(result, result_capacity);
                        }
                        memcpy(result + result_len, component_content, component_len);
                        result_len += component_len;

                        free(component_content);
                }

                // Move past the include tag
                curr_pos = include_end + 2;
        }

        // Append remaining text
        size_t suffix_len = strlen(curr_pos);
        if (result_len + suffix_len + 1 > result_capacity) {
                result_capacity = result_len + suffix_len + 1;
                result = realloc(result, result_capacity);
        }
        memcpy(result + result_len, curr_pos, suffix_len);
        result_len += suffix_len;
        result[result_len] = '\0';

        return result;
}

static char *inject_content(const char *template, const char *content) {
        if (!content) {
                return strdup(template);
        }

        const char *placeholder = "{{content}}";
        const char *placeholder_pos = strstr(template, placeholder);

        if (!placeholder_pos) {
                return strdup(template);
        }

        size_t prefix_len = placeholder_pos - template;
        size_t placeholder_len = strlen(placeholder);
        size_t content_len = strlen(content);
        size_t suffix_len = strlen(placeholder_pos + placeholder_len);

        char *result = malloc(prefix_len + content_len + suffix_len + 1);

        // Copy prefix
        memcpy(result, template, prefix_len);

        // Copy content
        memcpy(result + prefix_len, content, content_len);

        // Copy suffix
        memcpy(result + prefix_len + content_len, placeholder_pos + placeholder_len,
               suffix_len + 1);

        return result;
}

char *process_template(const char *filepath, const char *content) {
        // Check if this is a layout or regular page
        int is_layout = 0;
        const char *layout_prefix = "./src/web/layouts/";

        if (strncmp(filepath, layout_prefix, strlen(layout_prefix)) == 0) {
                is_layout = 1;
        }

        // Read the file content
        // char *file_content = read_file_contents(filepath);
        File *file = read_file_contents(filepath);
        if (!file) {
                return NULL;
        }

        // Process includes first
        char *processed = process_includes(file->content, 0);
        free(file);

        // If this is a regular page, check for a layout reference
        if (!is_layout) {
                const char *layout_tag = "{{layout:";
                const char *layout_start = strstr(processed, layout_tag);

                if (layout_start) {
                        const char *layout_end = strstr(layout_start, "}}");
                        if (layout_end) {
                                // Extract layout name
                                int layout_name_len =
                                    layout_end - (layout_start + strlen(layout_tag));
                                if (layout_name_len > 0 && layout_name_len < 255) {
                                        char layout_name[256] = {0};
                                        strncpy(layout_name, layout_start + strlen(layout_tag),
                                                layout_name_len);

                                        // Remove the layout tag from content
                                        char *new_content =
                                            malloc(strlen(processed) -
                                                   (strlen(layout_tag) + layout_name_len + 2) + 1);
                                        strncpy(new_content, processed, layout_start - processed);
                                        new_content[layout_start - processed] = '\0';
                                        strcat(new_content, layout_end + 2);

                                        // Full path to layout file
                                        char layout_path[512] = "./src/web/layouts/";
                                        strcat(layout_path, layout_name);
                                        if (!strstr(layout_path, ".html")) {
                                                strcat(layout_path, ".html");
                                        }

                                        // Process the layout with this content
                                        free(processed);
                                        processed = process_template(layout_path, new_content);
                                        free(new_content);

                                        return processed;
                                }
                        }
                }
        }

        // Inject content if provided (for layouts)
        if (content) {
                char *result = inject_content(processed, content);
                free(processed);
                return result;
        }

        return processed;
}

void free_template_cache() {
        // Free the template cache
        TemplateCache *entry = cache_head;
        while (entry) {
                TemplateCache *next = entry->next;
                free(entry->key);
                free(entry->content);
                free(entry);
                entry = next;
        }
        cache_head = NULL;
}
