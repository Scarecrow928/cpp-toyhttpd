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

uint16_t get_port() {
    uint32_t port_number = 12000;
    char *env_port = getenv("PORT");
    if (env_port != NULL) {
        port_number = std::stoul(std::string(env_port));
        if (port_number > 65535)
            port_number = 12000;
    }
    return port_number;
}

int get_maxevents() {
    int maxevents = 1024;
    char *env_maxevents = getenv("MAXEVENTS");
    if (env_maxevents != NULL) {
        maxevents = std::stoul(std::string(env_maxevents));
        if (maxevents > 65535)
            maxevents = 1024;
    }
    return maxevents;
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigterm_handler);
    HttpServer httpserver("0.0.0.0", get_port(), get_maxevents());
    httpserver.startup();
    return 0;
}
