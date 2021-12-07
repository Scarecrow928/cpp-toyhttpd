#include "test.h"
#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include <string>

test::test()
{

}

#define MAX_EVENTS 16
#define READ_SIZE 4096

int epollfd;

static void sigint_handler(int signo) {
    printf(" %d received.\n", signo);
    close(epollfd);
    exit(0);
}

// 每个fd所关联的上下文
typedef struct Context {
    std::string ip;
    uint16_t port;
    int fd;
    Context(const std::string &_ip, uint16_t _port, int _fd) :
        ip(_ip), port(_port), fd(_fd) {}
} Context;

void epoll_example() {
    int listen_sock, conn_sock;
    struct epoll_event ev, events[MAX_EVENTS];

    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0", &sa.sin_addr.s_addr);
    sa.sin_port = htons(19000);

    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == bind(listen_sock, (struct sockaddr *)&sa, sizeof(sa))) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (-1 == listen(listen_sock, 1024)) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // 来自epoll手册的建议框架
    epollfd = epoll_create(MAX_EVENTS);
    if (epollfd == -1) {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }
    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        int nfds = epoll_wait(epollfd, events, MAX_EVENTS, 300);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == listen_sock) {
                // 收到连接请求
                struct sockaddr_in addr;
                uint32_t addrlen = sizeof(addr);
                conn_sock = accept4(listen_sock, (struct sockaddr *) &addr, &addrlen, SOCK_NONBLOCK);
                if (conn_sock == -1) {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }

                char charip[20];
                inet_ntop(AF_INET, &addr.sin_addr.s_addr, charip, 20);
                std::string ip(charip);
                uint16_t port = ntohs(addr.sin_port);
                printf("accept: %s:%d\n", ip.data(), port);
                fflush(stdout);

                ev.events = EPOLLIN | EPOLLET;
                ev.data.ptr = (void *)new Context(ip, port, conn_sock);
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
                    perror("epoll_ctl: conn_sock");
                    exit(EXIT_FAILURE);
                }
            } else if (events[n].events == EPOLLIN) {
                // fd可读
                Context *ctx = (Context *)events[n].data.ptr;
                std::string row_request;
                char buf[READ_SIZE];
                int read_n;
                while ((read_n = recv(ctx->fd, buf, READ_SIZE - 1, 0)) > 0) {
                    buf[read_n] = '\0';
                    row_request += std::string(buf);
                }

                if (row_request.size() == 0) {
                    // 读取大小为0代表客户端关闭
                    printf("%s:%u closed.\n", ctx->ip.data(), ctx->port);
                    fflush(stdout);
                    close(ctx->fd);
                    delete ctx;
                } else {
                    printf("from %s:%u recv %lu bytes, content: %s\n", ctx->ip.data(), ctx->port,
                           row_request.size(), row_request.data());
                    fflush(stdout);
                }
            }
        }
    }
    for (auto &e : events) {
        if (e.data.ptr)
            delete (Context *)e.data.ptr;
    }
    close(epollfd);
}

void request_test() {
    Request r("aa");
    printf("r1...%s\n", r.valid == false ? "ok" : "failure");

    r = Request("GET / HTTP/1.1\r\nContent-Length: 0\r\n\r\n");
    printf("r2...%s\n", (r.method == "GET" &&
                         r.path == "" &&
                         r.version == "HTTP/1.1" &&
                         r.body.empty() &&
                         r.options.size() == 1 &&
                         r.params.size() == 0) ? "ok" : "failure");
    for (const auto &i : r.options)
        printf("%s: %s\n", i.first.data(), i.second.data());
}

void response_test() {
    std::string path = "hello.txt";
    Response r(path);
    char buf[8];
    size_t n;
    while ((n = r.next(buf, 7)) > 0) {
        buf[n] = '\0';
        printf("%s", buf);
        fflush(stdout);
    }
    printf("\n");
}

int main(int argc, char * argv[]) {
    request_test();
    // response_test();
    return 0;
}
