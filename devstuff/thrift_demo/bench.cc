#include "gen-cpp/echo.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

class echoHandler : virtual public echoIf {
    public:
        echoHandler() {
            // Your initialization goes here
        }

        void echo(hi& _return, const hi& msg) {
            // printf("echo\n");
            _return = msg;
        }

};

int main(int argc, char **argv) {
    if (argc == 2 && argv[1][1] == 'c') {
        boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
        boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        echoClient client(protocol);
        try {
            transport->open();
            hi out;
            hi ret;
            for (int i = 0; i < 1000 * 100; i++) {
                client.echo(ret, out);
            }
            printf("RET: %s\n", ret.msg.c_str());
            transport->close();
        } catch (TException& tx) {
            printf("ERROR: %s\n", tx.what());
        }
    } else {
        int port = 9090;
        shared_ptr<echoHandler> handler(new echoHandler());
        shared_ptr<TProcessor> processor(new echoProcessor(handler));
        shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
        shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
        shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
        TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
        server.serve();
        return 0;

    }
}
