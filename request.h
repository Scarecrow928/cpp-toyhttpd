#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <map>
#include <set>

class Request
{

public:
    std::string raw_request;
    std::string method;
    std::string version;
    std::string path;
    std::map<std::string, std::string> params, options;
    std::string body;
    bool valid = false;
    Request(const std::string &raw);

private:
    void url_filter();
    bool resolve();
    static const std::set<std::string> METHODS;
};

#endif // REQUEST_H
