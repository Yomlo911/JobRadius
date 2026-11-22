#pragma once

#include <cmath>
#include <iostream>
#include <string>

using namespace std;

static int passed = 0, failed = 0;
//checks if the test passed or not
inline void check(bool ok, const string& name) {
    cout << (ok ? "  PASS  " : "  FAIL  ") << name << "\n";
    if (ok) passed++; else failed++;
}

inline bool near(double a, double b, double eps = 0.01) { return fabs(a - b) < eps; }
//returns 0 if all tests are passed, otherwise 1
inline int finish() {
    cout << "\nResult: " << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}
