#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <folly/SocketAddress.h>
#include <iostream>

using namespace folly;
using namespace proxygen;
using namespace std;

class MyHandler: public proxygen::RequestHandler {
public:
    void onRequest(std::unique_ptr<HTTPMessage> headers) noexcept {
        // cout << headers->getURL() << endl;
    }

    void onBody(std::unique_ptr<folly::IOBuf> body) noexcept {
    }

    void onUpgrade(UpgradeProtocol protocol) noexcept {
    }

    void onEOM() noexcept {
        ResponseBuilder(downstream_)
            .status(200, "OK")
            .header("ALOHA", "blahhhh")
            .body("<h1>THIS WORKS!</h1>")
            .sendWithEOM();
    }

    void requestComplete() noexcept {
        delete this;
    }
    void onError(ProxygenError err) noexcept {
        delete this;
    }
};

class MyHandlerFactory : public RequestHandlerFactory {
public:
    void onServerStart(folly::EventBase* evb) noexcept override { }

    void onServerStop() noexcept override { }

    RequestHandler* onRequest(RequestHandler*, HTTPMessage*) noexcept override {
        return new MyHandler;
    }
};

int main(int argc, char* argv[]) {
    std::vector<HTTPServer::IPConfig> IPs = {
        {SocketAddress("0.0.0.0", 9988, true), proxygen::HTTPServer::Protocol::HTTP},
    };

    HTTPServerOptions options;
    options.threads = 32;
    options.idleTimeout = std::chrono::milliseconds(60000);
    options.shutdownOn = {SIGINT, SIGTERM};
    options.enableContentCompression = true;
    options.handlerFactories = RequestHandlerChain()
        .addThen<MyHandlerFactory>()
        .build();

    HTTPServer server(std::move(options));
    server.bind(IPs);
    server.start();
    return 0;
}
