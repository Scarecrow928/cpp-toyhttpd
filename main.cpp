#include <signal.h>

#include "httpserver.h"

static void sigint_handler(int signo) {
    printf("\nCtrl-C received, exit.\n");
    exit(0);
}

static void sigterm_handler(int signo) {
    printf("\nSIGTERM received, exit.\n");
    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigterm_handler);
    HttpServer httpserver;
    httpserver.startup();
    return 0;
}
