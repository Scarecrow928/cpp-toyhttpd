#include "response.h"

Response::Response(int code) {
    this->code = code;
}

void Response::add_header_option(const string &key, const string &value) {
    res_header_options[key] = value;
}

void Response::set_code(int code) {
    if (code >= 100 && code <= 511) {
        this->code = code;
        if (code >= 400) {
#ifdef DEBUG
            printf("set code %d\n", code);
#endif
            serve_default_file(code);
        }
    }
}

int Response::get_code() {
    return code;
}

string Response::serve_file(const string &file_) {
    this->file = file_;
    struct stat file_stat;
    if (0 != stat(file.data(), &file_stat)) {
        set_code(404);
        return file;
    } 
    
    if (S_ISDIR(file_stat.st_mode)) {
        if (file.back() != '/')
            file.push_back('/');
        file += INDEX_HTML;
    }

    // 或许可以列出一个目录
    if (0 != stat(file.data(), &file_stat)) {
        set_code(404);
        return file;
    } 

    ifstream fin(file);
    size_t dot = file.find_last_of(".");
    string file_extension;
    if (dot == string::npos) {
        file_extension = "txt";
    } else {
        file_extension = file.substr(dot + 1);
    }

    if (MyUtil::MIME_DICT.count(file_extension) == 0) {
#ifdef DEBUG
        printf("Can't solve: %s\n", file_extension.data());
#endif
        set_code(500);
        return file;
    }
    add_header_option("Content-Type", MyUtil::MIME_DICT[file_extension]);
    add_header_option("Content-Length", to_string(file_stat.st_size));
    return file;
}

void Response::serve_default_file(int code) {
    file = "default/" + to_string(code) + ".html";
#ifdef DEBUG
    printf("file is set to: %s\n", file.data());
#endif
}

string Response::get_file() {
    return file;
}

string Response::get_header() {
    // 准备回复了，加个时间戳
    add_header_option("Date", MyUtil::get_date("+%a, %d %b %Y %T GMT"));
    string header = "HTTP/1.1 " + to_string(code) + " " + MyUtil::STATUS_CODE[code] + "\r\n";
    for (auto it : res_header_options)
        header += it.first + ": " + it.second + "\r\n";
    header += "\r\n";
    return header;
}