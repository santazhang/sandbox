#include <iostream>

#include "rpc.h"

using namespace std;
using namespace rpc;

class myserver: public server {
public:
    i32 add32(const i32& a, const i32& b) {
        return a + b;
    }
};

int main() {
    myserver s;
    i32 add32 = 0x7001;
    s.reg(add32, &s, &myserver::add32);
    marshall m;
    unmarshall u;
    m << (i32) add32 << (i32) 3 << (i32) 4;
    s.invoke(m, u);
    i32 v;
    u >> v;
    cout << "result = " << v << endl;
    return 0;
}

