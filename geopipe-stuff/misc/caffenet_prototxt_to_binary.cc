#include "caffe/proto/caffe.pb.h"

#include <stdio.h>
#include <string>

#include <google/protobuf/text_format.h>
#include <glog/logging.h>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s prototxt_file output_binary_file\n", argv[0]);
        return 1;
    }
    FILE* fp = fopen(argv[1], "r");
    const int buf_size = 8192;
    char buf[buf_size];
    string prototxt;
    for (;;) {
        int cnt = fread(buf, 1, buf_size, fp);
        if (cnt <= 0) {
            break;
        }
        prototxt += string(buf, cnt);
    }
    fclose(fp);

    caffe::NetParameter net_param;
    CHECK(google::protobuf::TextFormat::ParseFromString(prototxt, &net_param));

    string proto_binary;
    CHECK(net_param.SerializeToString(&proto_binary));

    fp = fopen(argv[2], "wb");
    fwrite(proto_binary.data(), 1, proto_binary.length(), fp);
    fclose(fp);

    return 0;
}
