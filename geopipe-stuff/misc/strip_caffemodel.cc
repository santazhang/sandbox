#include "caffe/proto/caffe.pb.h"

#include <stdio.h>
#include <string>

#include <google/protobuf/text_format.h>
#include <glog/logging.h>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s caffe_model_file output_stripped_file\n", argv[0]);
        return 1;
    }
    FILE* fp = fopen(argv[1], "r");
    const int buf_size = 8192;
    char buf[buf_size];
    string proto_binary;
    for (;;) {
        int cnt = fread(buf, 1, buf_size, fp);
        if (cnt <= 0) {
            break;
        }
        proto_binary += string(buf, cnt);
    }
    fclose(fp);

    caffe::NetParameter net_param;
    CHECK(net_param.ParseFromString(proto_binary));
    string prototxt;
    CHECK(google::protobuf::TextFormat::PrintToString(net_param, &prototxt));
    // printf("%s\n", prototxt.c_str());
    // printf("--\n");

    caffe::NetParameter stripped_net = net_param;
    stripped_net.mutable_layer()->Clear();
    for (const auto& layer : net_param.layer()) {
        if (layer.type() == "Convolution" || layer.type() == "InnerProduct") {
            *(stripped_net.mutable_layer()->Add()) = layer;
        }
    }
    // prototxt = "";
    // CHECK(google::protobuf::TextFormat::PrintToString(stripped_net, &prototxt));
    // printf("%s\n", prototxt.c_str());
    string stripped;
    CHECK(stripped_net.SerializeToString(&proto_binary));

    fp = fopen(argv[2], "wb");
    fwrite(proto_binary.data(), 1, proto_binary.length(), fp);
    fclose(fp);

    return 0;
}
