#pragma once

#include <inttypes.h>
#include <map>
#include <iostream>

namespace rpc {

typedef int32_t i32;
typedef int64_t i64;
typedef char byte;

typedef i32 err_code;
typedef i32 svc_id_t;

const err_code NOERR = 0;

class bytebuffer {
	
};

struct marshall: private bytebuffer {
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

struct unmarshall: private bytebuffer  {
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

class server_endpoint {
    
    struct handler {
        virtual err_code handle(marshall& m, unmarshall& u) = 0;
    };
    
    std::map<svc_id_t, handler*> handlers;

public:

    err_code invoke(marshall& m, unmarshall& u);

    template<class S, class R, class T1, class T2>
    err_code reg(svc_id_t svc_id, S* svc, err_code (S::*svc_func)(const T1&, const T2&, R&)) {
        class h: public handler {
            S* s;
            err_code (S::*f)(const T1&, const T2&, R&);
        public:
            h(S* s, err_code (S::*f)(const T1&, const T2&, R&)): s(s), f(f) {}
            err_code handle(marshall& m, unmarshall& u) {
                T1 a;
                T2 b;
                m >> a >> b;
                R r;
                err_code ret = (s->*f)(a, b, r);
                u << r;
				return ret;
            }
        };
        this->handlers[svc_id] = new h(svc, svc_func);
        return NOERR;
    }

    template<class S, class R, class T1, class T2, class T3>
    err_code reg(svc_id_t svc_id, S* svc, err_code (S::*svc_func)(const T1&, const T2&, const T3&, R&)) {
        class h: public handler {
            S* s;
            err_code (S::*f)(const T1&, const T2&, const T3&, R&);
        public:
            h(S* s, err_code (S::*f)(const T1&, const T2&, const T3&, R&)): s(s), f(f) {}
            err_code handle(marshall& m, unmarshall& u) {
                T1 a;
                T2 b;
                T3 c;
                m >> a >> b >> c;
                R r;
                err_code ret = (s->*f)(a, b, c, r);
                u << r;
    			return ret;
            }
        };
        this->handlers[svc_id] = new h(svc, svc_func);
        return NOERR;
    }

};


class client {
};

}

