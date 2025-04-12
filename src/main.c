/* main.c */

#include "server/mime.h"
#include "server/server.h"

int main() {
        // allocate values commonly used
        init_mime_types();

        start_server();

        // deallocate values commonly used
        free_mime_types();

        return 0;
}
