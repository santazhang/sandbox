#include "find_trees.h"
#include "merge_trees.h"
#include "caffe_models.h"
#include "colors.h"

#include <stdio.h>
#include <math.h>

#include <vector>

namespace {

using std::vector;
using namespace find_trees;


const std::vector<caffe::Blob<float>*>& NetForward(
        caffe::Net<float>* net, const std::vector<caffe::Blob<float>*>& bottom, float* loss) {

    // hack to get rid of useless warning messages (deprecated caffe function call)
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

    result->tile_size = tile_width;
    CHECK_EQ(tile_width, tile_height);
    result->tile_rows = (params.img_height + tile_height - 1) / tile_height;
    result->tile_cols = (params.img_width + tile_width - 1) / tile_width;
    result->tile_step_x = step_x;
    result->tile_step_y = step_y;

    const int tile_pixels = tile_height * tile_width;
    caffe::Blob<float> blob(1 /* batch size = 1 */, num_channels, tile_height, tile_width);
    float* blob_raw = reinterpret_cast<float*>(blob.mutable_cpu_data());

    int tile_id = 0;
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
                result->tree_tiles.push_back(tile_id);
            }

            tile_id++;
        }
    }

    return 0;
}


int find_using_7x7_rgb_1(const params_t& params, result_t* result) {
    CHECK_GE(params.img_width, 7);
    CHECK_GE(params.img_height, 7);

    caffe::Net<float>* caffe_net_7x7_rgb_1 = get_caffe_net_7x7_rgb_1();

    const uint8_t* channel_ptrs[3] = {
        params.channel_red, params.channel_green, params.channel_blue
    };
    return find_generic(caffe_net_7x7_rgb_1, params, result,
                        3 /* num_channels*/, 7 /* tile_width */, 7 /* tile_height */,
                        7 /* step_x */, 7 /* step_y */,
                        channel_ptrs);
}

int find_using_7x7_rgb_lab_a_1(const params_t& params, result_t* result) {
    CHECK_GE(params.img_width, 7);
    CHECK_GE(params.img_height, 7);

    caffe::Net<float>* caffe_net_7x7_rgb_lab_a_1 = get_caffe_net_7x7_rgb_lab_a_1();

    const int n_pixels = params.img_width * params.img_height;
    uint8_t* lab_a_channel = new uint8_t[n_pixels];

    fill_lab_a_channel(n_pixels, lab_a_channel, params.channel_red, params.channel_green, params.channel_blue);

    const uint8_t* channel_ptrs[4] = {
        params.channel_red, params.channel_green, params.channel_blue, lab_a_channel
    };
    int ret = find_generic(caffe_net_7x7_rgb_lab_a_1, params, result,
                           4 /* num_channels*/, 7 /* tile_width */, 7 /* tile_height */,
                           7 /* step_x */, 7 /* step_y */,
                           channel_ptrs);

    delete[] lab_a_channel;

    return ret;
}


}  // anonymous namespace


namespace find_trees {

int find(const params_t& params, result_t* result) {
    CHECK_GT(params.img_width, 0);
    CHECK_GT(params.img_height, 0);
    CHECK_GT(params.max_tree_radius, 0);

    int ret = find_using_7x7_rgb_1(params, result);

    merge_trees(params, result);

    return ret;
}

}  // namespace find_trees
