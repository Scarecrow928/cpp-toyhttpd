#include "request.h"

const std::set<std::string> Request::METHODS  = {"GET", "POST", "HEAD"};

Request::Request(const std::string &raw) :
    raw_request(raw)
{
    valid = resolve();
}

void Request::url_filter()
{
    while (path.size() > 0 && path.back() == '/')
        path.pop_back();
    if (path.find("..") != std::string::npos)
        path = "";
}

bool Request::resolve()
{
    size_t idx = 0;
    for (; idx < raw_request.size() && method.size() <= 6; idx++) {
        if (raw_request[idx] == ' ')
            break;
        method.push_back(raw_request[idx]);
    }
    idx++;

    if (METHODS.count(method) == 0)
        return false;

    for (; idx < raw_request.size() && path.size() < 1024; idx++) {
        if (raw_request[idx] == ' ' || raw_request[idx] == '?')
            break;
        path.push_back(raw_request[idx]);
    }
    url_filter();
    if (raw_request[idx] == '?') {
        idx++;
        for (; raw_request[idx] != '\r';) {
            std::string k, v;
            for (; idx < raw_request.size() && k.size() < 1024; idx++) {
                if (raw_request[idx] == '=')
                    break;
                k.push_back(raw_request[idx]);
            }
            for (; idx < raw_request.size() && v.size() < 1024; idx++) {
                if (raw_request[idx] == '&' || raw_request[idx] == '\r')
                    break;
                v.push_back(raw_request[idx]);
            }
            params[k] = v;
        }
        if (raw_request[++idx] != '\n')
            return false;
    }
    idx++;

    for (; idx < raw_request.size() && version.size() < 12; idx++) {
        if (raw_request[idx] == '\r')
            break;
        version.push_back(raw_request[idx]);
    }
    if (raw_request[++idx] != '\n')
        return false;
    idx++;

    for (; raw_request[idx] != '\r';) {
        std::string k, v;
        for (; idx < raw_request.size() && k.size() < 1024; idx++) {
            if (raw_request[idx] == ':')
                break;
            k.push_back(raw_request[idx]);
        }
        idx++;
        while (raw_request[idx] == ' ')
            idx++;
        for (; idx < raw_request.size() && v.size() < 1024; idx++) {
            if (raw_request[idx] == '\r')
                break;
            v.push_back(raw_request[idx]);
        }
        options[k] = v;
        if (raw_request[++idx] != '\n')
            return false;
        idx++;
    }
    if (raw_request[++idx] != '\n')
        return false;
    idx++;
    body = raw_request.substr(idx);
    return true;
}
