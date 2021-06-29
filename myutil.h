#ifndef MYUTIL_H
#define MYUTIL_H

#include <string>
#include <unordered_map>

using namespace std;

class MyUtil {
public:
    static unordered_map<int, string> STATUS_CODE;
    static unordered_map<string, string> MIME_DICT;
    static string get_date();
};

#endif // MYUTIL_H