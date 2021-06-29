#ifndef MYUTIL_H
#define MYUTIL_H

#include <string>
#include <unordered_map>

using namespace std;

class MyUtil {
public:
    static unordered_map<int, string> STATUS_CODE;
    static unordered_map<string, string> MIME_DICT;

    // man date 查看format，需要加号
    static string get_date(const string &format);
};

#endif // MYUTIL_H