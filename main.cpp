#include <signal.h>

#include "httpserver.h"

static void sigint_handler(int signo) {
    printf("\nexit\n");
    exit(0);
}

int main()
{
    HttpServer httpserver;
    httpserver.startup();
    return 0;
}
