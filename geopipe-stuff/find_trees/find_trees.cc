#include "find_trees.h"
#include "merge_trees.h"
#include "caffe_models.h"
#include "colors.h"
#include "las_stats.h"

#include <stdio.h>
#include <math.h>

#include <vector>
#include <algorithm>

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

int find_generic2(
        caffe::Net<float>* caffe_net,
        const params_t& params,
        result_t* result,
        int num_channels,
        int tile_width,
        int tile_height,
        int step_x,
        int step_y,
        const uint8_t* channel_ptrs[],
        int num_float_channels,
        const double* float_ptrs[]) {

    result->tile_size = tile_width;
    CHECK_EQ(tile_width, tile_height);
    result->tile_rows = (params.img_height + tile_height - 1) / tile_height;
    result->tile_cols = (params.img_width + tile_width - 1) / tile_width;
    result->tile_step_x = step_x;
    result->tile_step_y = step_y;

    const int tile_pixels = tile_height * tile_width;
    caffe::Blob<float> blob(1 /* batch size = 1 */, num_channels + num_float_channels, tile_height, tile_width);
    // printf("  *** blob dim: 1 %d %d %d\n", num_channels + num_float_channels, tile_height, tile_width);
    float* blob_raw = reinterpret_cast<float*>(blob.mutable_cpu_data());

    int skip_mask_cnt = 0;
    CHECK_GE(num_channels, 3);

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


            // RGB intensity skip if TOTALLY BLACK OR WHITE (MASK COLOR)
            int mask_cnt = 0;
            for (int y = top_y, offst = 0, offst2 = y * params.img_width;
                 y < top_y + tile_height;
                 y++, offst += tile_width, offst2 += params.img_width) {

                for (int x = left_x; x < left_x + tile_width; x++) {
                    int v = channel_ptrs[0][offst2 + x] + channel_ptrs[1][offst2 + x] + channel_ptrs[2][offst2 + x];
                    if (v > (255 + 255 + 255) * 0.95 || v < (255 + 255 + 255) * 0.05) {
                        mask_cnt++;
                    }
                }
            }
            if (double(mask_cnt) / tile_pixels > 0.7) {
                skip_mask_cnt++;
                tile_id++;
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
                        // printf(" %d --- %d (img channel = %d)\n", channel_offst + offst + x_diff, offst2 + x, channel);
                        blob_raw[channel_offst + offst + x_diff] = channel_ptrs[channel][offst2 + x];
                    }
                }
            }
            
            // float channels
            if (num_float_channels > 0) {
                for (int y = top_y, offst = 0, offst2 = y * params.img_width; y < top_y + tile_height; y++, offst += tile_width, offst2 += params.img_width) {
                    for (int x = left_x, x_diff = 0; x < left_x + tile_width; x++, x_diff++) {
                        for (int flt_channel = 0, channel_offst = num_channels * tile_pixels;
                             flt_channel < num_float_channels;
                             flt_channel++, channel_offst += tile_pixels) {

                            // printf(" %d --- %d (float channel = %d)\n", channel_offst + offst + x_diff, offst2 + x, flt_channel);
                            blob_raw[channel_offst + offst + x_diff] = float_ptrs[flt_channel][offst2 + x];
                        }
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
    
    if (skip_mask_cnt > 0) {
        LOG(INFO) << "Skipped " << skip_mask_cnt << " possible mask tiles";
    }

    return 0;
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

    return find_generic2(caffe_net, params, result, num_channels, tile_width, tile_height, step_x, step_y, channel_ptrs,
                         0 /* float channels */, nullptr /* float channel ptrs */);
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

int find_using_7x7_rgb_ir_1(const params_t& params, result_t* result) {
    CHECK_GE(params.img_width, 7);
    CHECK_GE(params.img_height, 7);

    caffe::Net<float>* caffe_net_7x7_rgb_1 = get_caffe_net_7x7_rgb_ir_1();

    const uint8_t* channel_ptrs[4] = {
        params.channel_red, params.channel_green, params.channel_blue, params.channel_ir
    };
    return find_generic(caffe_net_7x7_rgb_1, params, result,
                        4 /* num_channels*/, 7 /* tile_width */, 7 /* tile_height */,
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

int find_using_7x7_rgb_las_z_hint_1(const params_t& params, result_t* result, const double* las_z_hint) {
    CHECK_GE(params.img_width, 7);
    CHECK_GE(params.img_height, 7);

    caffe::Net<float>* caffe_net_7x7_rgb_las_z_hint_1 = get_caffe_net_7x7_rgb_las_z_hint_1();

    const uint8_t* channel_ptrs[3] = {
        params.channel_red, params.channel_green, params.channel_blue
    };
    const double* float_channel_ptrs[1] = {
        las_z_hint
    };

    int ret = -1;
    ret = find_generic2(caffe_net_7x7_rgb_las_z_hint_1, params, result,
                        3 /* num_channels*/, 7 /* tile_width */, 7 /* tile_height */,
                        7 /* step_x */, 7 /* step_y */,
                        channel_ptrs, 1 /* float channels */, float_channel_ptrs);

    return ret;
}

void filter_result_using_z_hint(const params_t& params, result_t* result, const double* las_z_hint) {
    const double min_z_hint = 1.0; // TODO: hard coded for now
    const double max_z_hint = 12.5; // TODO: hard coded for now
    LOG(INFO) << "Filtering tree tiles with z hint bound = [" << min_z_hint << ", " << max_z_hint << "]";
    double* z_hint_values = new double[result->tile_size * result->tile_size];

    for (size_t idx = 0; idx < result->tree_tiles.size(); /* empty */) {
        int tile_id = result->tree_tiles[idx];
        const int top_y = (tile_id / result->tile_cols) * result->tile_step_y;
        const int left_x = (tile_id % result->tile_cols) * result->tile_step_x;
        int z_hint_count = 0;
        for (int y = top_y; y < top_y + result->tile_size && y < params.img_height; y++) {
            for (int x = left_x; x < left_x + result->tile_size && x < params.img_width; x++) {
                z_hint_values[z_hint_count] = las_z_hint[y * params.img_width + x];
                z_hint_count++;
            }
        }
        std::nth_element(z_hint_values, z_hint_values + z_hint_count/2, z_hint_values + z_hint_count);
        const double median = z_hint_values[z_hint_count/2];
        if (median < min_z_hint || median > max_z_hint) {
            result->tree_tiles[idx] = result->tree_tiles.back();
            result->tree_tiles.pop_back();
            result->trees[idx] = result->trees.back();
            result->trees.pop_back();
            continue;
        }
        idx++;
    }

    delete[] z_hint_values;
}

}  // anonymous namespace


namespace find_trees {

int find(const params_t& params, result_t* result) {
    CHECK_GT(params.img_width, 0);
    CHECK_GT(params.img_height, 0);
    CHECK_GT(params.max_tree_crown_diameter, 0);
    CHECK_GT(params.resolution, 0);

    double* las_z_hint = nullptr;
    if (params.points != nullptr) {
        int points_img_width = ceilf((params.points_max_x - params.points_min_x) / params.resolution);
        int points_img_height = ceilf((params.points_max_y - params.points_min_y) / params.resolution);

        LOG(INFO) << "points approx dimension: " << points_img_width << " x " << points_img_height;
        bool can_match_up = true;
        if (points_img_width != params.img_width || points_img_height != params.img_height) {
            // restrict distortion within 1%
            if (fabs(double(points_img_width) / params.img_width - 1.0) > 0.01) {
                can_match_up = false;
            }
            if (fabs(double(points_img_height) / params.img_height - 1.0) > 0.01) {
                can_match_up = false;
            }
            if (can_match_up) {
                points_img_width = params.img_width;
                points_img_height = params.img_height;
                LOG(INFO) << "points approx dimension adjusted to: " << points_img_width << " x " << points_img_height;
            } else {
                LOG(WARNING) << "points dimension does not match up with image dimension, will not use";
            }
        }

        if (can_match_up) {
            las_z_hint = new double[params.img_width * params.img_height];

            CHECK(params.points != nullptr);
            points_stats(params.points, params.n_points,
                         params.points_min_x, params.points_max_x, params.points_min_y, params.points_max_y,
                         params.resolution, points_img_width, points_img_height,
                         /*OUT*/ las_z_hint);
        }
    }

    int ret = -1;

    if (params.channel_ir == nullptr) {
        ret = find_using_7x7_rgb_1(params, result);
    } else {
        ret = find_using_7x7_rgb_ir_1(params, result);
    }

    if (las_z_hint != nullptr) {
        int before = result->trees.size();
        filter_result_using_z_hint(params, result, las_z_hint);
        int after = result->trees.size();
        LOG(INFO) << "Filtered " << (before - after) << " tree tiles using LAS points";
    }

    merge_trees(params, result);

    if (las_z_hint != nullptr) {
        delete[] las_z_hint;
    }

    return ret;
}

}  // namespace find_trees
