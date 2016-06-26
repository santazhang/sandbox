#pragma once

#ifndef CPU_ONLY
#define CPU_ONLY
#endif  // CPU_ONLY

#include <caffe/caffe.hpp>

#include <string>

namespace find_trees {

caffe::Net<float>* get_caffe_net_7x7_rgb_1();

}  // namespace find_trees
