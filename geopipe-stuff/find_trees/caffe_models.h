#pragma once

#include "caffe/proto/caffe.pb.h"

#include <string>

namespace find_trees {

std::string get_prototxt_net_param_7x7_rgb_1();
caffe::NetParameter get_net_param_7x7_rgb_1();
caffe::NetParameter get_trained_weights_7x7_rgb_1();

}  // namespace find_trees
