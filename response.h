#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>
#include <map>
#include <stdexcept>

#include <sys/stat.h>
#include <ctime>
#include <cstring>
#include <algorithm>
#include <cstdio>

class Response
{
public:
    int code = 200;
    size_t idx_header = 0, idx_file = 0, content_length;
    std::FILE *pf;
    std::string header, path;
    std::map<std::string, std::string> options;
    static std::map<int, std::string> STATUS_CODE;
    static std::map<std::string, std::string> GUESS_CONTENT_TYPE;

    Response(const std::string &_path, int _code = 200);
    ~Response();

    size_t next(char *buf, size_t _size);

private:
    void init_options();
    void guess_content_type();
    void init_content_length();
    void init_date();
};

#endif // RESPONSE_H
