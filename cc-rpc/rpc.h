#pragma once

#include <inttypes.h>
#include <map>
#include <iostream>

using namespace std;

namespace rpc {



typedef int32_t i32;
typedef int64_t i64;
typedef char byte;
typedef i32 protocol;

struct marshall {
    byte buf[1024];
    int put_idx;
    int get_idx;
    marshall() : put_idx(0), get_idx(0) {}
};

inline marshall& operator<<(marshall& m, const i32& i) {
    m.buf[m.put_idx] = (byte) (i & 0xFF);
    m.buf[m.put_idx + 1] = (byte) ((i >> 8) & 0xFF);
    m.buf[m.put_idx + 2] = (byte) ((i >> 16) & 0xFF);
    m.buf[m.put_idx + 3] = (byte) ((i >> 24) & 0xFF);
    m.put_idx += sizeof(i32);
    return m;
}

struct unmarshall {
    byte buf[1024];
    int get_idx;
    int put_idx;
    unmarshall() : get_idx(0), put_idx(0) {}
};

inline unmarshall& operator<<(unmarshall& m, const i32& i) {
    m.buf[m.put_idx] = (byte) (i & 0xFF);
    m.buf[m.put_idx + 1] = (byte) ((i >> 8) & 0xFF);
    m.buf[m.put_idx + 2] = (byte) ((i >> 16) & 0xFF);
    m.buf[m.put_idx + 3] = (byte) ((i >> 24) & 0xFF);
    m.put_idx += sizeof(i32);
    return m;
}


inline unmarshall& operator>>(unmarshall& u, i32& i) {
    i = (u.buf[u.get_idx] | (u.buf[u.get_idx + 1] << 8) | (u.buf[u.get_idx + 2] << 16) | (u.buf[u.get_idx + 3] << 24));
    u.get_idx += sizeof(i32);
    return u;
}

inline marshall& operator>>(marshall& u, i32& i) {
    i = (u.buf[u.get_idx] | (u.buf[u.get_idx + 1] << 8) | (u.buf[u.get_idx + 2] << 16) | (u.buf[u.get_idx + 3] << 24));
    u.get_idx += sizeof(i32);
    return u;
}
class server {
    class handler {
    public:
        virtual void fn(marshall& m, unmarshall& u) = 0;
    };
    std::map<protocol, handler*> handlers;
public:

    void invoke(marshall& m, unmarshall& u);

    template<class S, class R, class T1, class T2>
    bool reg(protocol p, S* s, R (S::*_func)(const T1&, const T2&)) {
        cout << "reged" << endl;
        class h: public handler {
            S* s;
            R (S::*func)(const T1&, const T2&);
        public:
            h(S* s, R (S::*_f)(const T1&, const T2&)): s(s), func(_f) {}
            void fn(marshall& m, unmarshall& u) {
                cout << "calling" << endl;
                T1 a;
                T2 b;
                m >> a;
                m >> b;
                cout << "fn: " << a << " " << b << endl;
                R r;
                r = (s->*func)(a, b);
                u << r;
            }
        };
        this->handlers[p] = new h(s, _func);
        return true;
    }

};


class client {
};

}

