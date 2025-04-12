/* router.h */

#ifndef ROUTER_H
#define ROUTER_H

void handle_route(int client_fd, const char *method, const char *path,
                  const char *query, const char *body);

#endif
