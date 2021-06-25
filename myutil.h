#ifndef MYUTIL_H
#define MYUTIL_H

#include <string>
#include <unordered_map>
#include <ctime>

using namespace std;

class MyUtil {
public:
    static unordered_map<int, string> STATUS_CODE;
    static unordered_map<string, string> MIME_DICT;
    static string get_date();
    static string addr_to_string(uint32_t addr);
    static uint32_t string_to_addr(const string &ip);
};

#endif // MYUTIL_H