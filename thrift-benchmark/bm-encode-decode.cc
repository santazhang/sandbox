#include <thrift/lib/cpp/util/ThriftSerializer.h>
#include <string>
#include <stdio.h>
#include <unistd.h>

#include "messages_types.h"

using apache::thrift::util::ThriftSerializerCompact;
using std::string;

int main() {
    ThriftSerializerCompact<> serializer;
    string output;
    Book book;
    book.author = "big boss";
    book.title = "hello world";
    book.pub_year = 2014;
    serializer.serialize(book, &output);
    printf("encoded size=%ld\n", output.size());

    Book book2;

    const int n_count = 1000 * 1000;

    struct timeval tv_start;
    gettimeofday(&tv_start, nullptr);

    for (int i = 0; i < n_count; i++) {
        serializer.serialize(book, &output);
        serializer.deserialize(output, &book2);
    }

    struct timeval tv_end;
    gettimeofday(&tv_end, nullptr);

    double secs = tv_end.tv_sec - tv_start.tv_sec +
                    (tv_end.tv_usec - tv_start.tv_usec) / 1000.0 / 1000.0;

    printf("encode decode qps: %lf\n", n_count / secs);

    return 0;
}

