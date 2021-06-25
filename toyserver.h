#ifndef TOYSERVER_H
#define TOYSERVER_H

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "requesthandler.h"

using namespace std;

void *thread_task(void *arg);

class ToyServer{
private:
    int httpd;

public:
    ToyServer();
    ~ToyServer();

    void bind_and_listen(const string &address, uint16_t port);
    void startup();

    void handle_request(int client);
};

#endif // TOYSERVER_H