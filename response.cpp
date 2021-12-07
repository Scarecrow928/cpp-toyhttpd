#include "response.h"

std::map<int, std::string> Response::STATUS_CODE = {
    {200, "OK"},

    {400, "Bad Request"},
    {404, "Not Found"},

    {500, "Internal Server Error"}
};

std::map<std::string, std::string> Response::GUESS_CONTENT_TYPE = {
    {"html", "text/html"},
    {"htm", "text/html"},
    {"txt", "text/plain"},
    {"js", "text/javascript"},
    {"css", "text/css"},
    {"png", "image/png"},
    {"jpg", "image/jpg"},
    {"jpeg", "image/jpg"},
    {"gif", "image/gif"},
    {"json", "application/json"},
    {"unknown", "application/octet-stream"}
};

Response::Response(const std::string &_path, int _code) :
    path(_path),
    code(_code)
{
    init_options();
    guess_content_type();
    init_content_length();
    init_date();
    header = "HTTP/1.1 " + std::to_string(code) + " " + STATUS_CODE[code] + "\r\n";
    for (const auto &i : options)
        header += i.first + ": " + i.second + "\r\n";
    header += "\r\n";
}

Response::~Response()
{
    std::fclose(pf);
}

size_t Response::next(char *buf, size_t _size)
{
    size_t idx_buf = 0;
    if (idx_header < header.size()) {
        size_t cp_size = std::min(header.size() - idx_header, _size);
        std::memcpy(buf, header.data() + idx_header, cp_size);
        idx_buf += cp_size;
        idx_header += cp_size;
    }

    if (idx_buf < _size && idx_file < content_length) {
        size_t cp_size = std::min(_size - idx_buf, content_length - idx_file);
        std::fread(buf + idx_buf, sizeof(char), cp_size, pf);
        idx_buf += cp_size;
        idx_file += cp_size;
    }
    return idx_buf;
}

void Response::init_options()
{
    options["Server"] = "myhttpd";
    options["Cache-Control"] = "no-cache";
    options["Connection"] = "Keep-Alive";
}

void Response::guess_content_type()
{
    std::string file_extension = "unknown";
    size_t d = path.find_last_of('.');
    if (d != std::string::npos)
        file_extension = path.substr(d + 1);
    if (GUESS_CONTENT_TYPE.count(file_extension) == 0)
        file_extension = "unknown";
    options["Content-Type"] = GUESS_CONTENT_TYPE[file_extension];
}

void Response::init_content_length()
{
    struct stat file_stat;
    if (0 != stat(path.data(), &file_stat))
        throw std::runtime_error("stat");
    content_length = file_stat.st_size;
    options["Content-Length"] = std::to_string(content_length);

    // open file
    pf = std::fopen(path.data(), "rb");
}

void Response::init_date()
{
    time_t time_now = time(nullptr);
    tm tm_now;
    gmtime_r(&time_now, &tm_now);
    char str_now[64];
    strftime(str_now, 64, "%a, %d %b %Y %T GMT", &tm_now);
    options["Date"] = std::string(str_now);
}

