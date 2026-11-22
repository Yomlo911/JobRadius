#include "test_helpers.h"
#include "../src/utils.h"

int main() {
    cout << "Helpers (utils.h)\n";
    check(toLower("C++") == "c++" && toLower("PYTHON") == "python", "toLower lowers letters");
    check(toLower("Udham-Singh 1") == "udham-singh 1", "toLower keeps digits, spaces and symbols");
    check(toLower("") == "", "toLower on empty string");

    check(trim("  a b  ") == "a b", "trim removes outer spaces and keeps inner ones");
    check(trim("\t x\n") == "x", "trim removes tabs and newlines");
    check(trim("") == "" && trim("   ") == "", "trim on empty and blank strings");
    check(trim("abc") == "abc", "trim leaves a clean string alone");

    check(allDigits("123") && allDigits("0"), "allDigits accepts digits");
    check(!allDigits(""), "allDigits rejects empty string");
    check(!allDigits("12a") && !allDigits("-5") && !allDigits(" 5") && !allDigits("4.5"), "allDigits rejects letters, signs, spaces and dots");
    return finish();
}
