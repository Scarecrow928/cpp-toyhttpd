#include <signal.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "toyserver.h"

using namespace std;

static void sigint_handler(int signo) {
    printf("SIGINT received, exit\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, sigint_handler);
    uint32_t port_number = 12000;

    char *env_port = getenv("PORT");
    if (env_port != NULL) {
        port_number = stoul(string(env_port));
        if (port_number > 65535)
            port_number = 12000;
    }
    ToyServer server;
    server.bind_and_listen("0.0.0.0", port_number);
    server.startup();
    return 0;
}