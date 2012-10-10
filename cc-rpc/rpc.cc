#include "rpc.h"

using namespace rpc;

err_code server_endpoint::invoke(marshall& m, unmarshall& u) {
    i32 p;
    m >> p;
    handler* h = this->handlers[p];
    return h->handle(m, u);
}

