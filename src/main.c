/* main.c */

#include "mime.h"
#include "radix_tree_router.h"
#include "server.h"
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void handle_fatal_signal(int sig) {
        void *bt[10];
        size_t size = backtrace(bt, 10);
        fprintf(stderr, "\nCaught signal %d. Stack trace:\n", sig);
        backtrace_symbols_fd(bt, size, STDERR_FILENO);
        _exit(1); // Avoid undefined behavior from continuing
}

int main() {
        // Ignore SIGPIPE to prevent crashes when writing to closed sockets
        signal(SIGPIPE, SIG_IGN);

        // Handle Ctrl+C and `kill`
        signal(SIGINT, shutdown_server);
        signal(SIGTERM, shutdown_server);

        // Log and exit on fatal signals
        signal(SIGSEGV, handle_fatal_signal);
        signal(SIGABRT, handle_fatal_signal);
        signal(SIGFPE, handle_fatal_signal);
        signal(SIGILL, handle_fatal_signal);

        // allocate values commonly used
        init_mime_types();
        init_router();

        start_server();

        // deallocate values commonly used
        free_router();
        free_mime_types();

        return 0;
}
