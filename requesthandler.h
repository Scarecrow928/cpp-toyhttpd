#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <string>
#include <unordered_map>
#include <fstream>
#include <utility>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "request.h"
#include "response.h"

using namespace std;

#define READ_SIZE 8192

class RequestHandler {
private:
    int client;
    Request req;
    Response res;
    string htdocs = "htdocs";
    
public:
    RequestHandler(int client);
    ~RequestHandler();

    void read_request();
    void process_request();
    void response();
};

#endif // REQUESTHANDLER_H