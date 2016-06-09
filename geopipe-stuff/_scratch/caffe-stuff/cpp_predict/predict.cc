#include <string>

#define CPU_ONLY

#include "caffe/caffe.hpp"
#include "caffe/util/io.hpp"
#include "caffe/blob.hpp"

using namespace caffe;
using namespace std;

int main(int argc, char* argv[]) {
    Caffe::set_mode(Caffe::CPU);
    
    Net<float> caffe_test_net(argv[1], TEST);
    caffe_test_net.CopyTrainedLayersFrom(argv[2]);

    Datum datum;
    if (!ReadImageToDatum("./p.bmp", 1, 7, 7, &datum)) {
        LOG(ERROR) << "LOAD NOT OK";
    }
    LOG(INFO) << "IMG LOAD OK";
    
    Blob<float>* blob = new Blob<float>(1, datum.channels(), datum.height(), datum.width());
  //get the blobproto
    BlobProto blob_proto;
    blob_proto.set_num(1);
    blob_proto.set_channels(datum.channels());
    blob_proto.set_height(datum.height());
    blob_proto.set_width(datum.width());
    const int data_size = datum.channels() * datum.height() * datum.width();
    int size_in_datum = std::max<int>(datum.data().size(),
                                      datum.float_data_size());
    for (int i = 0; i < size_in_datum; ++i) {
      blob_proto.add_data(0.);
    }
    const string& data = datum.data();
    if (data.size() != 0) {
      for (int i = 0; i < size_in_datum; ++i) {
        blob_proto.set_data(i, blob_proto.data(i) + (uint8_t)data[i]);
      }
    }
  
    //set data into blob
    blob->FromProto(blob_proto);

    //fill the vector
    vector<Blob<float>*> bottom;
    bottom.push_back(blob);
    float type = 0.0;

    const vector<Blob<float>*>& result =  caffe_test_net.Forward(bottom, &type);
    
    LOG(INFO) << "*** RESULTS";
    for (int i = 0; i < result.size() ; i++) {
        Blob<float>* b = result[i];
        if (b->cpu_data()[0] > b->cpu_data()[1]) {
            LOG(INFO) << "result -> NOT_TREE";
        } else {
            LOG(INFO) << "result -> TREE";
        }
    }
    
    return 0;
}
