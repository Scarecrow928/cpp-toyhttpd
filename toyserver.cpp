#include "toyserver.h"

using namespace std;
#define BACKLOG 1024

void *thread_task(int clientfd) {
    RequestHandler rh(clientfd);
    rh.read_request();
    rh.process_request();
    rh.response();
#ifdef DEBUG
    printf("thread exit\n");
#endif
    pthread_exit(NULL);
}

ToyServer::ToyServer() : pool(10000)
{
}

ToyServer::~ToyServer()
{
    shutdown(httpd, SHUT_RDWR);
}

void ToyServer::bind_and_listen(const string &address, uint16_t port)
{
    // TCP
    httpd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in listen_addr;
    listen_addr.sin_family = AF_INET;

    // IP字符串，端口转化为网络格式，也就是大端模式
    inet_pton(AF_INET, address.data(), &listen_addr.sin_addr.s_addr);
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
        int client = accept(httpd, (struct sockaddr *)&client_addr, &len);
        
        // 打印客户端信息
        char buf[20];
        inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, buf, 20);
        string ip = string(buf);
        uint16_t port = ntohs(client_addr.sin_port);
        printf("%s accept: %s:%d\n", MyUtil::get_date("%F %T", false).data(), ip.data(), port);
        fflush(stdout);
        // 启动线程处理
        handle_request(client);
    }
}

void ToyServer::handle_request(int client) {
    boost::asio::post(pool, [client]() {
        thread_task(client);
    });
}