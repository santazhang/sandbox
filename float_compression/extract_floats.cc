#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <queue>
#include <algorithm>
#include <iostream>
#include <stdint.h>
#include <map>

#include "caffe.pb.h"
#include "nn_lossy.h"

using google::protobuf::Message;
using google::protobuf::io::FileInputStream;
using google::protobuf::io::ZeroCopyInputStream;
using google::protobuf::io::CodedInputStream;
using namespace caffe;
using namespace std;

namespace {


const int kProtoReadBytesLimit = INT_MAX;  // Max size of 2 GB minus 1 byte.

bool ReadProtoFromBinaryFile(const char* filename, Message* proto) {
  int fd = open(filename, O_RDONLY);
  if (fd < 0) {
      printf("  *** FAILED TO OPEN FILE!\n");
  }
  ZeroCopyInputStream* raw_input = new FileInputStream(fd);
  CodedInputStream* coded_input = new CodedInputStream(raw_input);
  coded_input->SetTotalBytesLimit(kProtoReadBytesLimit, 536870912);

  bool success = proto->ParseFromCodedStream(coded_input);
  if (!success) {
      printf("  *** FAILED TO LOAD PROTO DATA!\n");
  }

  delete coded_input;
  delete raw_input;
  close(fd);
  return success;
}

union flt  {  
    struct ieee754 {
       unsigned int mantissa:23;
       unsigned int exponent:8;
       unsigned int sign:1;
    } raw;
    float f;
 };

void print_float_structure(float v) {
    uint32_t* pv = (uint32_t*) &v;
    uint32_t u = *pv;
    uint32_t sign = u >> 31;
    uint32_t expo = (u >> 23) & 0xFF;
    uint32_t frac = u & 0x7FFFFF;
    cout << v << " \tsign=" << sign << ", \texp=" << expo << ", \tfrac=";
    printf("%06x", frac);
    flt f;
    f.f = v;
    cout << "       \tsign=" << f.raw.sign << ", \texp="
            << f.raw.exponent << ", \tfrac=";
    printf("%06x\n", f.raw.mantissa);
}



// huffman tree
typedef std::vector<bool> HuffCode;
typedef std::pair<unsigned, int> sign_expo;
typedef std::map<sign_expo, HuffCode> HuffCodeMap;

class INode
{
public:
    const int f;
 
    virtual ~INode() {}
 
protected:
    INode(int f) : f(f) {}
};
 
class InternalNode : public INode
{
public:
    INode *const left;
    INode *const right;
 
    InternalNode(INode* c0, INode* c1) : INode(c0->f + c1->f), left(c0), right(c1) {}
    ~InternalNode()
    {
        delete left;
        delete right;
    }
};
 
class LeafNode : public INode
{
public:
    const sign_expo c;
 
    LeafNode(int f, sign_expo c) : INode(f), c(c) {}
};
 
struct NodeCmp
{
    bool operator()(const INode* lhs, const INode* rhs) const { return lhs->f > rhs->f; }
};
 
INode* BuildTree(std::map<std::pair<unsigned, int>, int>::iterator it, std::map<std::pair<unsigned, int>, int>::iterator it_end)
{
    std::priority_queue<INode*, std::vector<INode*>, NodeCmp> trees;
 
    while (it != it_end)
    {
        if(it->second != 0)
            trees.push(new LeafNode(it->second, it->first));
        ++it;
    }
    while (trees.size() > 1)
    {
        INode* childR = trees.top();
        trees.pop();
 
        INode* childL = trees.top();
        trees.pop();
 
        INode* parent = new InternalNode(childR, childL);
        trees.push(parent);
    }
    return trees.top();
}
 
void GenerateCodes(const INode* node, const HuffCode& prefix, HuffCodeMap& outCodes)
{
    if (const LeafNode* lf = dynamic_cast<const LeafNode*>(node))
    {
        outCodes[lf->c] = prefix;
    }
    else if (const InternalNode* in = dynamic_cast<const InternalNode*>(node))
    {
        HuffCode leftPrefix = prefix;
        leftPrefix.push_back(false);
        GenerateCodes(in->left, leftPrefix, outCodes);
 
        HuffCode rightPrefix = prefix;
        rightPrefix.push_back(true);
        GenerateCodes(in->right, rightPrefix, outCodes);
    }
}



void analyze1(const float* farr, int n_floats) {
    // (sign, expo) -> count
    std::map<std::pair<unsigned, int>, int> sign_expo_counter;
    for (int i = 0; i < n_floats; i++) {
        flt f;
        f.f = farr[i];
        sign_expo_counter[std::make_pair(f.raw.sign, f.raw.exponent - 128)]++;
    }
    INode* root = BuildTree(sign_expo_counter.begin(), sign_expo_counter.end());
    HuffCodeMap codes;
    GenerateCodes(root, HuffCode(), codes);

    // for (std::map<std::pair<unsigned, int>, int>::iterator it = sign_expo_counter.begin();
    //         it != sign_expo_counter.end(); ++it) {
    //      cout << "(" << it->first.first << ", " << it->first.second << ") -> " << it->second << endl;
    // }
    int compressed_len = 0;
    for (HuffCodeMap::iterator it = codes.begin(); it != codes.end(); ++it) {
        cout << "(" << it->first.first << ", " << it->first.second << ") -> " <<
            sign_expo_counter[it->first] << " -> ";
        for (size_t i = 0; i < it->second.size(); i++) {
            cout << it->second[i];
        }
        compressed_len += sign_expo_counter[it->first] * it->second.size();
        cout << endl;
    }
    printf("compressed len = %d, origin len = %d (%.2f%%)\n", compressed_len, n_floats * 9,
        100.0 * compressed_len / n_floats / 9.0);
    printf("============\n");
    
    delete root;
}

void analyze2(const float* farr, int n_floats) {
    // (sign, expo) -> count
    std::map<std::pair<unsigned, int>, int> sign_expo_counter;
    for (int i = 0; i < n_floats; i++) {
        flt f;
        f.f = farr[i];
        sign_expo_counter[std::make_pair(f.raw.sign, f.raw.exponent - 128)]++;
    }
    int pos_most_common_expo = -128;
    int neg_most_common_expo = -128;
    for (int i = -128 ; i < 128; i++) {
        if (sign_expo_counter[std::make_pair(0, i)] > sign_expo_counter[std::make_pair(0, pos_most_common_expo)]) {
            pos_most_common_expo = i;
        }
        if (sign_expo_counter[std::make_pair(1, i)] > sign_expo_counter[std::make_pair(1, neg_most_common_expo)]) {
            neg_most_common_expo = i;
        }
    }
    cout << "pos common=" << pos_most_common_expo << ", neg common = " << neg_most_common_expo << endl;
    std::map<std::pair<unsigned, int>, int> sign_expo_counter_filtered;
    for (std::map<std::pair<unsigned, int>, int>::iterator it = sign_expo_counter.begin(); 
            it != sign_expo_counter.end(); ++it) {
        ;
        const int cut_off = 7;
        if (it->first.first == 0) {
            // pos
            if (it->first.second >= pos_most_common_expo - cut_off) {
                sign_expo_counter_filtered[it->first] = it->second;
            } else {
                sign_expo_counter_filtered[std::make_pair(0, -128)] += it->second;
            }
        } else {
            // neg
            if (it->first.second >= neg_most_common_expo - cut_off) {
                sign_expo_counter_filtered[it->first] = it->second;
            } else {
                sign_expo_counter_filtered[std::make_pair(0, -128)] += it->second;
            }
        }
    }
    sign_expo_counter.swap(sign_expo_counter_filtered);

    INode* root = BuildTree(sign_expo_counter.begin(), sign_expo_counter.end());
    HuffCodeMap codes;
    GenerateCodes(root, HuffCode(), codes);

    // for (std::map<std::pair<unsigned, int>, int>::iterator it = sign_expo_counter.begin();
    //         it != sign_expo_counter.end(); ++it) {
    //      cout << "(" << it->first.first << ", " << it->first.second << ") -> " << it->second << endl;
    // }
    int compressed_len = 0;
    for (HuffCodeMap::iterator it = codes.begin(); it != codes.end(); ++it) {
        cout << "(" << it->first.first << ", " << it->first.second << ") -> " <<
            sign_expo_counter[it->first] << " -> ";
        for (size_t i = 0; i < it->second.size(); i++) {
            cout << it->second[i];
        }
        compressed_len += sign_expo_counter[it->first] * it->second.size();
        cout << endl;
    }
    printf("compressed len = %d, origin len = %d (%.2f%%)\n", compressed_len, n_floats * 9,
        100.0 * compressed_len / n_floats / 9.0);
    printf("============\n");
    
    delete root;
}

void analyze3(const float* farr, int n_floats) {
    int compressed_len = 0;
    int block_size = 256;
    for (int block_start = 0; block_start < n_floats; block_start += block_size) {
        // (sign, expo) -> count
        std::map<std::pair<unsigned, int>, int> sign_expo_counter;
        for (int i = block_start; i < n_floats && i < block_start + block_size; i++) {
            flt f;
            f.f = farr[i];
            sign_expo_counter[std::make_pair(f.raw.sign, f.raw.exponent - 128)]++;
        }
        INode* root = BuildTree(sign_expo_counter.begin(), sign_expo_counter.end());
        HuffCodeMap codes;
        GenerateCodes(root, HuffCode(), codes);

        // for (std::map<std::pair<unsigned, int>, int>::iterator it = sign_expo_counter.begin();
        //         it != sign_expo_counter.end(); ++it) {
        //      cout << "(" << it->first.first << ", " << it->first.second << ") -> " << it->second << endl;
        // }
    
        for (HuffCodeMap::iterator it = codes.begin(); it != codes.end(); ++it) {
            // cout << "(" << it->first.first << ", " << it->first.second << ") -> " <<
            //     sign_expo_counter[it->first] << " -> ";
            // for (size_t i = 0; i < it->second.size(); i++) {
            //     cout << it->second[i];
            // }
            compressed_len += sign_expo_counter[it->first] * it->second.size();
            // cout << endl;
        }
        delete root;
    }
    printf("compressed len = %d, origin len = %d (%.2f%%)\n", compressed_len, n_floats * 9,
        100.0 * compressed_len / n_floats / 9.0);
    printf("============\n");
    
    
}

void try_compress(const float* farr, int n_floats) {
    float* farr2 = new float[n_floats];
    printf("*** trying compression\n");
    char* compressed;
    int compressed_bytes;
    nn_compress(farr, n_floats, &compressed, &compressed_bytes);
    printf("*** trying decompression\n");
    nn_decompress(compressed, compressed_bytes, farr2, n_floats);
    printf("*** done compression-decompression\n");
}

void process_blob(const BlobProto& blob) {
    if (blob.width() < 16) {
        return;
    }
    printf("n=%d c=%d h=%d w=%d\n", blob.num(), blob.channels(), blob.height(), blob.width());
    const float* farr = blob.data().data();
    int n_floats = blob.data().size();
    // analyze1(farr, n_floats);
    // printf("**********\n");
    // analyze2(farr, n_floats);
    // printf("**********\n");
    // analyze3(farr, n_floats);
    // for (int i = 0; i < blob.width(); i++) {
        // print_float_structure(blob.data(i));
    // }
    try_compress(farr, n_floats);
}

}

int main(int argc, char* argv[]) {
    NetParameter net_param;
    if (argc == 1) {
        ReadProtoFromBinaryFile("VGG_CNN_F.caffemodel", &net_param);
    } else {
        ReadProtoFromBinaryFile(argv[1], &net_param);
    }
    cout << net_param.name() << endl;
    if (net_param.layers_size() > 0) {
        cout << "(v1) layers: " << net_param.layers_size() << endl;
        for (int i = 0; i < net_param.layers_size(); i++) {
            const ::caffe::V1LayerParameter& layer = net_param.layers(i);
            V1LayerParameter_LayerType lt = layer.type();
            if (layer.blobs_size() > 0) {
                cout << i << ": " << V1LayerParameter_LayerType_Name(lt) << endl;
            }
            for (int j = 0; j < layer.blobs_size(); j++) {
                const BlobProto& blob = layer.blobs(j);
                process_blob(blob);
            }
        }
    }
    if (net_param.layer_size() > 0) {
        cout << "layer: " << net_param.layer_size() << endl;
    }
    return 0;
}
