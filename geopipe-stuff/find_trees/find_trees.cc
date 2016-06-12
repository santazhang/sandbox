#include "find_trees.h"
#include "caffe_models.h"

#include <stdio.h>

#include <functional>

#ifndef CPU_ONLY
#define CPU_ONLY
#endif  // CPU_ONLY

#include <caffe/caffe.hpp>


namespace {

using namespace find_trees;

const std::vector<caffe::Blob<float>*>& NetForward(
        caffe::Net<float>* net, const std::vector<caffe::Blob<float>*>& bottom, float* loss) {

    // hack to avoid warning
    class MyNet : public caffe::Net<float> {
    public:
        void my_pre_forward(const std::vector<caffe::Blob<float>*>& a_bottom) {
            // Copy bottom to net bottoms
            for (size_t i = 0; i < a_bottom.size(); ++i) {
                net_input_blobs_[i]->CopyFrom(*a_bottom[i]);
            }
        }
    };

    MyNet* mynet = static_cast<MyNet*>(net);
    mynet->my_pre_forward(bottom);
    return mynet->Forward(loss);
}

int find_generic(
        caffe::Net<float>* caffe_net,
        const params_t& params,
        result_t* result,
        int num_channels,
        int tile_width,
        int tile_height,
        int step_x,
        int step_y,
        const uint8_t* channel_ptrs[]) {

    const int tile_pixels = tile_height * tile_width;
    caffe::Blob<float> blob(1 /* batch size = 1 */, num_channels, tile_height, tile_width);
    float* blob_raw = reinterpret_cast<float*>(blob.mutable_cpu_data());

    for (int top_y = 0; top_y < params.img_height; top_y += step_y) {
        if (top_y + tile_height > params.img_height) {
            top_y = params.img_height - tile_height;
        }
        if (top_y < 0) {
            continue;
        }

        for (int left_x = 0; left_x < params.img_width; left_x += step_x) {
            if (left_x + tile_width > params.img_width) {
                left_x = params.img_width - tile_width;
            }
            if (left_x < 0) {
                continue;
            }

            for (int y = top_y, offst = 0, offst2 = y * params.img_width;
                 y < top_y + tile_height;
                 y++, offst += tile_width, offst2 += params.img_width) {

                for (int x = left_x, x_diff = 0 ; x < left_x + tile_width; x++, x_diff++) {
                    // blob_raw[0 * 7 * 7 + y_diff * 7 + x_diff] = channel_red[tile_width * y + x];
                    // blob_raw[1 * 7 * 7 + y_diff * 7 + x_diff] = channel_green[tile_width * y + x];
                    // blob_raw[2 * 7 * 7 + y_diff * 7 + x_diff] = channel_blue[tile_width * y + x];
                    // blob_raw[0 * tile_pixels + offst + x_diff] = params.channel_red[offst2 + x];
                    // blob_raw[1 * tile_pixels + offst + x_diff] = params.channel_green[offst2 + x];
                    // blob_raw[2 * tile_pixels + offst + x_diff] = params.channel_blue[offst2 + x];
                    for (int channel = 0, channel_offst = 0; channel < num_channels; channel++, channel_offst += tile_pixels) {
                        blob_raw[channel_offst + offst + x_diff] = channel_ptrs[channel][offst2 + x];
                    }
                }
            }

            // predict!
            std::vector<caffe::Blob<float>*> bottom;
            bottom.push_back(&blob);
            float loss_value = 0.0;
            const std::vector<caffe::Blob<float>*>& predict_result = NetForward(caffe_net, bottom, &loss_value);
            caffe::Blob<float>* b = predict_result[0];
            if (b->cpu_data()[0] < b->cpu_data()[1]) {
                // TREE!
                tree_info_t t;
                t.x_pixels = left_x + 0.5 * tile_width;
                t.y_pixels = top_y + 0.5 * tile_height;
                t.radius_pixels = 0.5 * 1.414 * tile_width;  // assuming tile_width == tile_height
                result->trees.push_back(t);
            }
        }
    }
    
    return 0;
}


int find_using_7x7_rgb_1(const params_t& params, result_t* result) {
    static caffe::Net<float>* caffe_net_7x7_rgb_1 = nullptr;

    if (caffe_net_7x7_rgb_1 == nullptr) {
        caffe::NetParameter net_proto_7x7_rgb_1 = get_net_param_7x7_rgb_1();
        caffe::NetParameter trained_weight_7x7_rgb_1 = get_trained_weights_7x7_rgb_1();
        caffe_net_7x7_rgb_1 = new caffe::Net<float>(net_proto_7x7_rgb_1);
        caffe_net_7x7_rgb_1->CopyTrainedLayersFrom(trained_weight_7x7_rgb_1);
    }

    const uint8_t* channel_ptrs[3] = {
        params.channel_red, params.channel_green, params.channel_blue
    };
    return find_generic(caffe_net_7x7_rgb_1, params, result,
                        3 /* num_channels*/, 7 /* tile_width */, 7 /* tile_height */,
                        7 /* step_x */, 7 /* step_y */,
                        channel_ptrs);

    // const int step_y = 7;
    // const int step_x = 7;
    // const int tile_width = 7;
    // const int tile_height = 7;
    // const int tile_pixels = tile_height * tile_width;
    //
    // caffe::Blob<float> blob(1 /* batch size = 1 */, 3 /* channels, rgb */, tile_height, tile_width);
    // float* blob_raw = reinterpret_cast<float*>(blob.mutable_cpu_data());
    //
    // for (int top_y = 0; top_y < params.img_height; top_y += step_y) {
    //     if (top_y + tile_height > params.img_height) {
    //         top_y = params.img_height - tile_height;
    //     }
    //     if (top_y < 0) {
    //         continue;
    //     }
    //
    //     for (int left_x = 0; left_x < params.img_width; left_x += step_x) {
    //         if (left_x + tile_width > params.img_width) {
    //             left_x = params.img_width - tile_width;
    //         }
    //         if (left_x < 0) {
    //             continue;
    //         }
    //
    //         for (int y = top_y, offst = 0, offst2 = y * params.img_width;
    //              y < top_y + tile_height;
    //              y++, offst += tile_width, offst2 += params.img_width) {
    //
    //             for (int x = left_x, x_diff = 0 ; x < left_x + tile_width; x++, x_diff++) {
    //                 // blob_raw[0 * 7 * 7 + y_diff * 7 + x_diff] = channel_red[tile_width * y + x];
    //                 // blob_raw[1 * 7 * 7 + y_diff * 7 + x_diff] = channel_green[tile_width * y + x];
    //                 // blob_raw[2 * 7 * 7 + y_diff * 7 + x_diff] = channel_blue[tile_width * y + x];
    //                 blob_raw[0 * tile_pixels + offst + x_diff] = params.channel_red[offst2 + x];
    //                 blob_raw[1 * tile_pixels + offst + x_diff] = params.channel_green[offst2 + x];
    //                 blob_raw[2 * tile_pixels + offst + x_diff] = params.channel_blue[offst2 + x];
    //             }
    //         }
    //
    //         // predict!
    //         std::vector<caffe::Blob<float>*> bottom;
    //         bottom.push_back(&blob);
    //         float loss_value = 0.0;
    //         const std::vector<caffe::Blob<float>*>& predict_result = NetForward(caffe_net_7x7_rgb_1, bottom, &loss_value);
    //         caffe::Blob<float>* b = predict_result[0];
    //         if (b->cpu_data()[0] < b->cpu_data()[1]) {
    //             // TREE!
    //             tree_info_t t;
    //             t.x_pixels = left_x + 3.5;
    //             t.y_pixels = top_y + 3.5;
    //             t.radius_pixels = 4.9;  // ~sqrt(2)*(7/2)
    //             result->trees.push_back(t);
    //         }
    //     }
    // }
    //
    // return 0;
}

}  // anonymous namespace


namespace find_trees {

int find(const params_t& params, result_t* result) {
    int ret = find_using_7x7_rgb_1(params, result);

    // TODO merge trees!

    return ret;
}

}  // namespace find_trees
