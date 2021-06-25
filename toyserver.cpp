#include "toyserver.h"

using namespace std;
#define BACKLOG 1024

void *thread_task(void *arg) {
    int *fd = (int *)arg;
    RequestHandler rh(*fd);
    delete fd;
    rh.read_request();
    rh.process_request();
    rh.response();
#ifdef DEBUG
    printf("thread exit\n");
    fflush(stdout);
#endif
    pthread_exit(NULL);
}

ToyServer::ToyServer()
{
}

ToyServer::~ToyServer()
{
}

void ToyServer::bind_and_listen(const string &address, uint16_t port)
{
    // TCP
    httpd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in listen_addr;
    listen_addr.sin_family = AF_INET;

    // IP字符串转化为4字节uint32_t
    uint32_t ip = MyUtil::string_to_addr(address);
#ifdef DEBUG
    printf("listen ip: %u\n", ip);
#endif
    listen_addr.sin_addr.s_addr = ip;

    if (port < 0 || port > 65535) {
        printf("port number error\n");
        exit(1);
    }
    listen_addr.sin_port = htons(port);
    
    // 地址、端口绑定
    if (-1 == bind(httpd, (struct sockaddr *)&listen_addr, sizeof(listen_addr))) {
        perror("bind");
        exit(1);
    }
    
    // 开始监听
    if (-1 == listen(httpd, BACKLOG)) {
        perror("listen");
        exit(1);
    }
    printf("listen on %s:%d\n", address.data(), port);
}

void ToyServer::startup() {
    while (true) {
        struct sockaddr_in client_addr;
        uint32_t len = sizeof(client_addr);
        fflush(stdout);
        int client = accept(httpd, (struct sockaddr *)&client_addr, &len);
#ifdef DEBUG
        string ip = MyUtil::addr_to_string(client_addr.sin_addr.s_addr);
        uint16_t port = ntohs(client_addr.sin_port);
        printf("accept: %s:%d\n", ip.data(), port);
#endif
        handle_request(client);
    }
}

void ToyServer::handle_request(int client) {
    int *fd = new int(client);
    pthread_t pid;
    pthread_create(&pid, NULL, thread_task, fd);
}