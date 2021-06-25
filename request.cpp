#include "request.h"

Request::Request(const string &row_req) {
    this->row_req = row_req;

    // 解析请求方法
    size_t begin = 0;
    size_t end = row_req.find_first_of(" ", begin);
    if (end == string::npos)
        return;
    method = row_req.substr(begin, end - begin);
    begin = end + 1;

    while (begin < row_req.size() && row_req[begin] == ' ')
        begin++;

    // 解析请求路径
    end = row_req.find_first_of(" ", begin);
    if (end == string::npos)
        return;
    resolve_url(row_req.substr(begin, end - begin));
    begin = end + 1;

    while (begin < row_req.size() && row_req[begin] == ' ')
        begin++;

    // 解析HTTP协议版本
    end = row_req.find_first_of("\r\n", begin);
    if (end == string::npos)
        return;
    http_version = row_req.substr(begin, end - begin);
    begin = end + 2;
#ifdef DEBUG
    printf("version = %s\n", http_version.data());
#endif   

    // 解析头部选项
    for (; end != string::npos && begin < row_req.size(); begin = end + 2) {
        end = row_req.find_first_of("\r\n", begin);
        resolve_header_option(row_req.substr(begin, end - begin));
    }

    // 头部与正文之间的间隔
    if (begin >= row_req.size() || row_req.substr(begin, 2) != "\r\n") {
#ifdef DEBUG
        printf("Content is empty\n");
#endif
        return;
    }
    begin += 2;

    // 正文开始
}

void Request::resolve_url(const string &url) {
    size_t end = url.find_first_of("?");
    if (end == string::npos) {
        req_path = url;
    } else {
        req_path = url.substr(0, end);
        for (size_t begin = end + 1; end != string::npos && begin < url.size(); begin = end + 1) {
            pair<string, string> param;
            end = url.find_first_of("=", begin);
            if (end == string::npos)
                break;
            param.first = url.substr(begin, end - begin);
            begin = end + 1;
            if (begin >= url.size())
                break;
            end = url.find_first_of("&", begin);
            param.second = url.substr(begin, end - begin);
            req_params.insert(param);
        }
    }

    // 过滤url
    if (req_path.find("..") != string::npos)
        req_path = "/";

#ifdef DEBUG
    printf("resolve url = %s\n", req_path.data());
    for (auto it : req_params)
        printf("param = \'%s = %s\'\n", it.first.data(), it.second.data());
#endif
}

void Request::resolve_header_option(const string &line) {
    pair<string, string> option;
    size_t pos = line.find_first_of(":");
    if (pos == string::npos)
        return;
    option.first = line.substr(0, pos);
    pos++;
    while (line[pos] == ' ')
        pos++;
    if (pos >= line.size())
        return;
    option.second = line.substr(pos);
    req_headers.insert(option);
#ifdef DEBUG
    printf("header options = %s: %s\n", option.first.data(), option.second.data());
#endif
}