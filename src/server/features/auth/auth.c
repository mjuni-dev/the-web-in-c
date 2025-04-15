/* auth.c */

#include "auth.h"
#include "radix_tree_router.h"
#include "response.h"
#include <stdio.h>
#include <string.h>

void handle_signin_get(int client_fd, const char *query, const char *body);
void handle_signin_post(int client_fd, const char *query, const char *body);

void auth_register_routes() {
        add_route("GET", "/auth/signin", handle_signin_get);
        add_route("POST", "/auth/signin", handle_signin_post);
}

void handle_signin_get(int client_fd, const char *query, const char *body) {
        printf(" >> HANDLE__SIGNIN_GET()\n");

        // just removing warning from build for now.
        // I may change the signature.  I don't know if I will need a body for
        // get requests
        if (query && body) {
        }
        handle_static(client_fd, "/auth/signin.html");
}

void handle_signin_post(int client_fd, const char *query, const char *body) {
        printf(" >> HANDLE_SIGNIN_POST()\n");

        // just removing warning from build for now.
        // Will probably change the signature, I don't know that I will need a
        // query for all handlers
        if (query) {
        }

        char username[128] = {0};
        char passwd[128] = {0};

        char *pair = strtok((char *)body, "&");
        while (pair) {
                printf(" >> PAIR: %s\n", pair);
                char key[128], val[128];
                if (sscanf(pair, "%127[^=]=%127s", key, val) == 2) {
                        if (strcmp(key, "username") == 0) {
                                strncpy(username, val, sizeof(username) - 1);
                        } else if (strcmp(key, "password") == 0) {
                                strncpy(passwd, val, sizeof(passwd) - 1);
                        }
                }
                pair = strtok(NULL, "&");
        }

        printf("Sign-in attemp: username: '%s', password: '%s'\n", username,
               passwd);

        char resp[512];
        snprintf(resp, sizeof(resp), "<h1>Welcome, %s!</h1>", username);
        send_response(client_fd, "200 OK", "text/html", resp);
}
