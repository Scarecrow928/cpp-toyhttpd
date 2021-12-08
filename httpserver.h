#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <string>

#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

#include "request.h"
#include "response.h"

class HttpServer
{
public:
    std::string address, web_root = "/var/www/html/";
    uint16_t port;

    typedef struct Context {
        std::string ip, path;
        uint16_t port;
        int fd, epollfd, code;
        HttpServer *server;
        Context(const std::string &_ip, uint16_t _port, int _fd, int _epollfd, HttpServer *_server) :
            ip(_ip),
            port(_port),
            fd(_fd),
            epollfd(_epollfd),
            server(_server)
        {}
    } Context;

    HttpServer(const std::string _address = "0.0.0.0", uint16_t _port = 12000);
    ~HttpServer();
    void startup();

private:
    int listenfd, epollfd;
    boost::asio::thread_pool thread_pool;
    static const int MAX_EVENTS = 1024, READ_SIZE = 8192, WRITE_SIZE = 8192;

    void generate_default_page();
};

#endif // HTTPSERVER_H
