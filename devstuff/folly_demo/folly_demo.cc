#include <folly/Conv.h>
#include <folly/Unicode.h>

#include <iostream>
#include <string>

using namespace std;

int main() {
    cout << folly::to<string>(1.987) << endl;
    cout << folly::codePointToUtf8(128123).toStdString() << endl;
    return 0;
}
