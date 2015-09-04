/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */

// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "Echo.h"
#include <thrift/lib/cpp/async/TEventServer.h>
#include <thrift/lib/cpp/server/TConnectionContext.h>
#include <thrift/lib/cpp/util/TEventServerCreator.h>

using apache::thrift::TProcessor;
using apache::thrift::async::TEventServer;
using apache::thrift::util::TEventServerCreator;

using std::shared_ptr;

using namespace  ::test;

class EchoHandler : virtual public EchoIf {
 public:
  EchoHandler() {
    // Your initialization goes here
  }

  int32_t echo(int32_t v) {
    // Your implementation goes here
    printf("echo\n");
    return int32_t();
  }

};

int main(int argc, char **argv) {
  int port = 9090;
  shared_ptr<EchoHandler> handler(new EchoHandler());
  shared_ptr<TProcessor> processor(new EchoProcessor(handler));
  TEventServerCreator serverCreator(processor, port);
  shared_ptr<TEventServer> server = serverCreator.createEventServer();
  server->serve();
  return 0;
}