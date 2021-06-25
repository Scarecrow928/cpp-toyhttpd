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
    {"gif", "image/gif"}
};

string MyUtil::get_date() {
    // C中没有特别好的库获取当前时间，用date命令加管道获取
    char str[128];
    const string cmdstring = "date -u \'+%a, %d %b %Y %T GMT\'";
    FILE *f = popen(cmdstring.data(), "r");
    int n = fread(str, sizeof(char), 127, f);
    pclose(f);
    str[n] = '\0';
    string date_str(str);
    while (date_str.back() == '\n')
        date_str.pop_back();
    return date_str;
}

string MyUtil::addr_to_string(uint32_t addr) {
    string ip;
    uint8_t *b = (uint8_t *)&addr;
    for (int i = 0; i < 4; i++)
        ip += to_string(b[i]) + ".";
    ip.pop_back();
    return ip;
}

uint32_t MyUtil::string_to_addr(const string &ip) {
    uint32_t addr = 0;
    for (int i = 24, j = 0; i >= 0; i -= 8) {
        int k = ip.find_first_of('.', j);
        uint32_t a = stoul(ip.substr(j, k));
        if (a < 0 || a > 255) {
            printf("address error: %s\n", ip.data());
            return 0;
        }
        addr |= (a << i);
        j = k + 1;
    }
    return addr;
}