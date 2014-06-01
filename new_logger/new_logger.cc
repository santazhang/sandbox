#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

class Log {
};

class LogHelper {
public:
    ~LogHelper() {
        cout << endl;
    }
};

template <class T>
LogHelper operator << (const Log& L, const T& t) {
    LogHelper l;
    l << t;
    return l;
}

template <class T>
const LogHelper& operator << (const LogHelper& L, const T& t) {
    cout << t;
    return L;
}

class CheckLog {
    string msg_;
public:
    template <class T>
    CheckLog(const T t) {
        ostringstream os;
        os << t;
        msg_ = os.str();
    }
};



template <class T>
ostream& operator << (ostream& os, const T& t) {
    os << t;
    return os;
}


class Check {
    bool f_;
    vector<CheckLog> chk_;
public:
    Check(bool f): f_(f) {
        
    }
    bool get_check_result() {
        return f_;
    }
    ~Check();
    void add(const CheckLog& chk_log) {
        chk_.push_back(chk_log);
    }
};

class CheckWithMsg {
    bool f_;
    vector<CheckLog> chk_;
public:
    CheckWithMsg(bool f) {
        f_ = f;
    }
    ~CheckWithMsg() {
        if (!f_) {
            printf("AHA THIS SHOULD PRINT HTE LOG MESSAGE\n");
        }
    }
    void add(const CheckLog& chk_log) {
        chk_.push_back(chk_log);
    }
};

template <class T>
CheckWithMsg operator << (Check L, const T& t) {
    CheckWithMsg chk(L.get_check_result());
    chk.add(CheckLog(t));
    return chk;
}

Check::~Check() {
    if (!f_) {
        for (const auto& chk : chk_) {
            cout << chk;
        }
        cout << endl;
    }
}


int main() {
    Log() << "this should end with new line" << 3 << 4 << 5;
    Check(3 < 4);
    Check(3 > 4) << "oops";
    return 0;
}
