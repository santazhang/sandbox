#include "rpc.h"

using namespace rpc;

void server::invoke(marshall& m, unmarshall& u) {
    protocol p;
    m >> p;
    cout << "invoke!" << p << endl;
    handler* h = this->handlers[p];
    cout << "invoking " << h << endl;
    h->fn(m, u);
}

