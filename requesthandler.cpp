#include "requesthandler.h"

RequestHandler::RequestHandler(int client) {
    this->client = client;

    // 有些请求较慢，为防止recv阻塞，设置超时
    struct timeval tv = {0, 10000};
    setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
}

RequestHandler::~RequestHandler() {

}

void RequestHandler::read_request() {
    // 读出原始请求报文
    int n;
    char buf[READ_SIZE];
    string row_req;
    fflush(stdout);
    while ((n = recv(client, buf, READ_SIZE - 1, 0)) > 0) {
        buf[n] = '\0';
        row_req += string(buf);
    }
#ifdef DEBUG
    printf("request text: \n%s\nrequest size: %lu\n", row_req.data(), row_req.size());
#endif
    
    // 解析原始请求
    req = Request(row_req);
}

void RequestHandler::process_request() {
    if (req.method == "GET") {
        string path = htdocs + req.req_path;
        res.serve_file(path);
    } else {
        res.set_code(500);
    }
}

void RequestHandler::response() {
    // 发送头部
    string header = res.get_header();
#ifdef DEBUG
    printf("ready to send header:\n%s", header.data());
#endif
    fflush(stdout);
    send(client, header.data(), header.size(), 0);

    // 发送内容
    char buf[READ_SIZE];
    int n;
    string serve_file_name = res.get_file();
#ifdef DEBUG
    printf("ready to send file: %s\n", serve_file_name.data());
    fflush(stdout);
    size_t read_count = 0, send_count = 0;
#endif
    FILE *pf = fopen(serve_file_name.data(), "rb");
    fflush(stdout);
    while ((n = fread(buf, sizeof(char), READ_SIZE, pf)) > 0) {
#ifdef DEBUG
        read_count += n;
#endif
        if (-1 == (n = send(client, buf, n, 0))) {
            perror("send");
            break;
        }
#ifdef DEBUG
        send_count += n;
#endif
    }
    fclose(pf);
#ifdef DEBUG
    printf("read file \"%s\" %ld Bytes, sent: %ld Bytes\n", serve_file_name.data(), read_count, send_count);
#endif
    shutdown(client, SHUT_RDWR);
#ifdef DEBUG
    printf("close client connection\n");
#endif
    fflush(stdout);
}

