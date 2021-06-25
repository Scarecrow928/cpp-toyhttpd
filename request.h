#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <unordered_map>
#include <filesystem>

using namespace std;

class Request {
public:
    string row_req;
    string method;
    string req_path;
    unordered_map<string, string> req_params;
    string http_version;
    unordered_map<string, string> req_headers;

    Request(const string &row_req = "");
    void resolve_url(const string &url);
    void resolve_header_option(const string &line);
};

#endif // REQUEST_H