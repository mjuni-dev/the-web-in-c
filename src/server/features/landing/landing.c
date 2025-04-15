/* landing.c */

#include "landing.h"
#include "radix_tree_router.h"
#include "response.h"
#include <stdio.h>

void handle_root_get(int client_fd, const char *query, const char *body);
void handle_contactus_get(int client_fd, const char *query, const char *body);
void handle_aboutus_get(int client_fd, const char *query, const char *body);

void landing_register_routes() {
        add_route("GET", "/", handle_root_get);
        add_route("GET", "/contactus", handle_contactus_get);
        add_route("GET", "/aboutus", handle_aboutus_get);
}

void handle_root_get(int client_fd, const char *query, const char *body) {
        printf(" >> HANDLE_ROOT_GET()\n");

        // just removing warning from build for now.
        // I may change the signature.  I don't know if I will need a body for
        // get requests
        if (query && body) {
        }
        handle_static(client_fd, "/index.html");
}

void handle_contactus_get(int client_fd, const char *query, const char *body) {
        printf(" >> HANDLE_CONTACTUS_GET()\n");

        // just removing warning from build for now.
        // I may change the signature.  I don't know if I will need a body for
        // get requests
        if (query && body) {
        }
        handle_static(client_fd, "/contactus.html");
}

void handle_aboutus_get(int client_fd, const char *query, const char *body) {
        printf(" >> HANDLE_ABOUTUS_GET()\n");

        // just removing warning from build for now.
        // I may change the signature.  I don't know if I will need a body for
        // get requests
        if (query && body) {
        }
        handle_static(client_fd, "/aboutus.html");
}
