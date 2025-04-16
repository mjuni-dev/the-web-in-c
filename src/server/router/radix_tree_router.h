/* radix_tree_router.H */

#ifndef RADIX_ROUTER_H
#define RADIX_ROUTER_H

typedef void (*RouteHandler)(int client_fd, const char *query,
                             const char *body);

typedef struct s_radix_node {
        char *segment;
        RouteHandler handler;
        struct s_radix_node **children;
        int child_count;
} RadixNode;

typedef struct {
        const char *method;
        RadixNode *root;
} MethodTree;

void add_route(const char *method, const char *path, RouteHandler handler);
RouteHandler find_route(const char *method, const char *path);

void init_router();
void free_router();

#endif
