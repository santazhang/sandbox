#include <iostream>

#include "my-rpc.h"

using namespace std;
using namespace rpc;

struct my_add_service {
    i32 add32(const i32& a, const i32& b, i32& r) {
        r = a + b;
        return NOERR;
    }
};

struct my_sub_service {
    i32 sub32(const i32& a, const i32& b, i32& r) {
        r = a - b;
        return NOERR;
    }
};

struct my_mul_service {
    i32 mul32(const i32& a, const i32& b, const i32& c, i32& r) {
        r = a * b * c;
        return NOERR;
    }
};

int main() {
    server_endpoint s;
    my_add_service my_add;
    my_sub_service my_sub;
    my_mul_service my_mul;
    s.reg(my_protocol::add32, &my_add, &my_add_service::add32);
    s.reg(my_protocol::sub32, &my_sub, &my_sub_service::sub32);
    s.reg(my_protocol::mul32, &my_mul, &my_mul_service::mul32);
    i32 v;
    for (int i = 0; i < 10000000; i++) {
        marshall m;
        unmarshall u;
        m << (i32) my_protocol::mul32 << (i32) 3 << (i32) 4 << (i32) 8;
        err_code ecode = s.invoke(m, u);
        u >> v;
    }
    cout << "result = " << v << endl;
    return 0;
}

