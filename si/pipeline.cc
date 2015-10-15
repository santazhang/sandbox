#include <iostream>
#include <string>

#include <stdlib.h>

using namespace std;

template <typename D>
class Downstream {
public:
    virtual void send(const D&) = 0;
};

template <typename U>
class Upstream {
public:
    virtual void maybe_recv(U*) = 0;
};

template <typename U, typename D>
class Pipeline : virtual public Upstream<D>, virtual public Downstream<U> {
public:
    void set_downstream(Downstream<D>* downstream) {
        downstream_ = downstream;
    }
protected:
    Upstream<U>* upstream_ = NULL;
    Downstream<D>* downstream_ = NULL;
};

template <typename U, typename D>
class Protocol : public Pipeline<U, D> {
};

class IntToDouble : public Protocol<int, double> {
public:
    void send(const int& v) {
        cout << "int -> double: " << v << endl;
        if (downstream_ != NULL) {
            downstream_->send(v * 3.14);
        }
    }
    void maybe_recv(double*) {
        
    }
};

class DoubleToString : public Protocol<double, string> {
public:
    void send(const double& v) {
        cout << "double -> string: " << v << endl;
        if (downstream_ != NULL) {
            downstream_->send("aloha");
        }
    }
    void maybe_recv(string*) {
        
    }
};

class StringToLongerString : public Protocol<string, string> {
public:
    void send(const string& v) {
        cout << "string -> longer string: " << v << endl;
    }
    void maybe_recv(string*) {
        
    }
};

int main() {
    IntToDouble out1;
    DoubleToString out2;
    StringToLongerString out3;
    out1.set_downstream(&out2);
    out2.set_downstream(&out3);
    out1.send(34);
    return 0;
}
