#pragma once

#include <string>

using namespace std;

inline string toLower(const string& s) {
    string out = s;
    for (size_t i = 0; i < out.size(); i++) {
        if (out[i] >= 'A' && out[i] <= 'Z') out[i] = out[i] - 'A' + 'a';
    }
    return out;
}

inline string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

inline bool allDigits(const string& s) {
    if (s.empty()) return false;
    for (size_t i = 0; i < s.size(); i++) if (s[i] < '0' || s[i] > '9') return false;
    return true;
}
