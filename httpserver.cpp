#include "httpserver.h"

HttpServer::HttpServer(const std::string _address, uint16_t _port) :
    address(_address),
    port(_port),
    thread_pool(10000)
{
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    // listen at address:port
    struct sockaddr_in listen_addr;
    listen_addr.sin_family = AF_INET;
    inet_pton(AF_INET, address.data(), &listen_addr.sin_addr.s_addr);
    listen_addr.sin_port = htons(port);

    // bind
    if (-1 == bind(listenfd, (struct sockaddr *)&listen_addr, sizeof(listen_addr))) {
        perror("bind");
        exit(1);
    }

    // listen
    if (-1 == listen(listenfd, 4096)) {
        perror("listen");
        exit(1);
    }
}

HttpServer::~HttpServer()
{
    close(listenfd);
    close(epollfd);
}

void HttpServer::startup()
{
    generate_default_page();
    printf("Start HttpServer at %s:%u", address.data(), port);
    fflush(stdout);
    epollfd = epoll_create(MAX_EVENTS);
    if (epollfd == -1) {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }

    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
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
            if (events[n].data.fd == listenfd) {
                // 收到连接请求
                struct sockaddr_in addr;
                uint32_t addrlen = sizeof(addr);
                int conn_sock = accept4(listenfd, (struct sockaddr *) &addr, &addrlen, SOCK_NONBLOCK);
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
                ev.data.ptr = (void *)new Context(ip, port, conn_sock, epollfd, this);
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
                    perror("epoll_ctl: conn_sock");
                    exit(EXIT_FAILURE);
                }
            } else if (events[n].events == EPOLLIN) {
                // fd可读
                Context *ctx = (Context *)events[n].data.ptr;
                boost::asio::post(thread_pool, [ctx]() {
                    std::string row_request;
                    char buf[READ_SIZE];
                    int read_n;
                    while ((read_n = recv(ctx->fd, buf, READ_SIZE - 1, 0)) > 0) {
                        buf[read_n] = '\0';
                        row_request += std::string(buf);
                    }
                    if (row_request.size() == 0) {
                        // 连接关闭
                        printf("close: %s:%u\n", ctx->ip.data(), ctx->port);
                        fflush(stdout);
                        close(ctx->fd);
                        delete ctx;
                    } else {
                        // 处理请求
                        Request request(row_request);
                        std::string path = ctx->server->web_root + "error.html";
                        request.path = ctx->server->web_root + request.path;
                        int code;
                        if (request.valid) {
                            struct stat file_stat;
                            if (0 != stat(request.path.data(), &file_stat)) {
                                code = 404;
                            } else if (S_ISDIR(file_stat.st_mode)) {
                                request.path += "/index.html";
                                if (0 != stat(request.path.data(), &file_stat)) {
                                    code = 404;
                                } else {
                                    code = 200;
                                    path = request.path;
                                }
                            } else {
                                code = 200;
                                path = request.path;
                            }
                        } else {
                            code = 400;
                        }
                        ctx->path = path;
                        ctx->code = code;

                        // 改为监听客户端fd的可写
                        struct epoll_event write_ev;
                        write_ev.events = EPOLLOUT | EPOLLET;
                        write_ev.data.ptr = (void *)ctx;
                        if (epoll_ctl(ctx->epollfd, EPOLL_CTL_MOD, ctx->fd, &write_ev) == -1) {
                            perror("epoll_ctl: read ctx->fd");
                            exit(EXIT_FAILURE);
                        }
                    }
                });
            } else if (events[n].events == EPOLLOUT) {
                // fd可写
                Context *ctx = (Context *)events[n].data.ptr;
                boost::asio::post(thread_pool, [ctx]() {
                    Response response(ctx->path, ctx->code);
                    char buf[WRITE_SIZE];
                    size_t n;
                    while ((n = response.next(buf, READ_SIZE)) > 0) {
                        if (-1 == (n = send(ctx->fd, buf, n, 0))) {
                            perror("send");
                            break;
                        }
                    }

                    // 改为监听客户端fd可读
                    struct epoll_event write_ev;
                    write_ev.events = EPOLLIN | EPOLLET;
                    write_ev.data.ptr = (void *)ctx;
                    if (epoll_ctl(ctx->epollfd, EPOLL_CTL_MOD, ctx->fd, &write_ev) == -1) {
                        perror("epoll_ctl: write ctx->fd");
                        exit(EXIT_FAILURE);
                    }
                });
            }
        }
    }
    for (auto &e : events) {
        if (e.data.ptr)
            delete (Context *)e.data.ptr;
    }
    close(epollfd);
}

void HttpServer::generate_default_page()
{
    struct stat file_stat;
    std::string error_page = web_root + "error.html", index_page = web_root + "index.html";
    if (0 != stat(error_page.data(), &file_stat)) {
        int new_file = open(error_page.data(), O_RDWR | O_CREAT, 0644);
        if (new_file != -1)
            close(new_file);
        else
            perror("open");
    }
    if (0 != stat(index_page.data(), &file_stat)) {
        int new_file = open(index_page.data(), O_RDWR | O_CREAT, 0644);
        if (new_file != -1)
            close(new_file);
        else
            perror("open");
    }
}
