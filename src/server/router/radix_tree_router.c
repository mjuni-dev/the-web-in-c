/* radix_tree_router.c */

#include "radix_tree_router.h"
#include "auth.h"
#include "landing.h"
#include <stdlib.h>
#include <string.h>

#define MAX_METHODS 4
static MethodTree method_trees[MAX_METHODS];
static int method_count = 0;

static RadixNode *create_node(const char *segment) {
        RadixNode *node = malloc(sizeof(RadixNode));
        node->segment = strdup(segment);
        node->handler = NULL;
        node->children = NULL;
        node->child_count = 0;
        return node;
}

static RadixNode *get_method_root(const char *method) {
        for (int i = 0; i < method_count; i++) {
                if (strcmp(method_trees[i].method, method) == 0) {
                        return method_trees[i].root;
                }
        }

        // Create a new method tree
        if (method_count < MAX_METHODS) {
                method_trees[method_count].method = strdup(method);
                method_trees[method_count].root = create_node("");
                return method_trees[method_count++].root;
        }

        return NULL;
}

static void add_child(RadixNode *parent, RadixNode *child) {
        parent->children = realloc(
            parent->children, sizeof(RadixNode *) * (parent->child_count + 1));
        parent->children[parent->child_count++] = child;
}

void add_route(const char *method, const char *path, RouteHandler handler) {
        RadixNode *node = get_method_root(method);
        if (!node) {
                return;
        }

        char temp[1024];
        strncpy(temp, path, sizeof(temp));
        char *token = strtok(temp, "/");

        while (token) {
                int found = 0;
                for (int i = 0; i < node->child_count; i++) {
                        if (strcmp(node->children[i]->segment, token) == 0) {
                                node = node->children[i];
                                found = 1;
                                break;
                        }
                }

                if (!found) {
                        RadixNode *new_node = create_node(token);
                        add_child(node, new_node);
                        node = new_node;
                }

                token = strtok(NULL, "/");
        }

        node->handler = handler;
}

RouteHandler find_route(const char *method, const char *path) {
        RadixNode *node = get_method_root(method);
        if (!node) {
                return NULL;
        }

        char temp[1024];
        strncpy(temp, path, sizeof(temp));
        char *token = strtok(temp, "/");

        while (token) {
                int found = 0;
                for (int i = 0; i < node->child_count; i++) {
                        if (strcmp(node->children[i]->segment, token) == 0) {
                                node = node->children[i];
                                found = 1;
                                break;
                        }
                }

                if (!found) {
                        return NULL;
                }

                token = strtok(NULL, "/");
        }

        return node->handler;
}

void init_router() {
        method_count = 0;
        auth_register_routes();
        landing_register_routes();
}
void free_router() { method_count = 0; }
