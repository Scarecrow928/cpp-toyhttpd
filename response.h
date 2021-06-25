#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>
#include <fstream>
#include <unordered_map>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "myutil.h"

using namespace std;

class Response {
private:
    const string INDEX_HTML = "index.html";
    int code;
    unordered_map<string, string> res_header_options = {
        {"Server", "toyhttpd/0.1"},
        {"Cache-Control", "no-cache"},
        {"Connection", "close"}
    };
    string file;
    void serve_default_file(int code);

public:
    Response(int code = 200);
    void add_header_option(const string &key, const string &value);
    void set_code(int code);
    int get_code();
    string serve_file(const string &file);
    string get_file();
    string get_header();
};

#endif // RESPONSE_H