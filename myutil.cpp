#include "myutil.h"

unordered_map<int, string> MyUtil::STATUS_CODE = {
    {200, "OK"},
    {404, "Not Found"},
    {500, "Internal Server Error"}
};

unordered_map<string, string> MyUtil::MIME_DICT = {
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

string MyUtil::get_date_(const string &format) {
    // 用date命令加管道获取当前时间
    char str[128];
    const string cmdstring = "date -u \'+" + format + "\'";
    FILE *f = popen(cmdstring.data(), "r");
    int n = fread(str, sizeof(char), 127, f);
    pclose(f);
    str[n] = '\0';
    string date_str(str);
    while (date_str.back() == '\n')
        date_str.pop_back();
    return date_str;
}

string MyUtil::get_date(const string &format, bool gmt) {
    // 用localtime，gmtime，strftime获取当前时间字符串
    time_t time_now = time(nullptr);
    tm tm_now;
    if (gmt)
        gmtime_r(&time_now, &tm_now);
    else
        localtime_r(&time_now, &tm_now);
    char str_now[64];
    strftime(str_now, 64, format.data(), &tm_now);
    return string(str_now);
}