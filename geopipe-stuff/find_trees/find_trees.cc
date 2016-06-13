#include "find_trees.h"
#include "caffe_models.h"

#include <stdio.h>
#include <math.h>

#include <functional>
#include <vector>

#ifndef CPU_ONLY
#define CPU_ONLY
#endif  // CPU_ONLY

#include <caffe/caffe.hpp>


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


struct private_tree_info_t {
    tree_info_t ti;
};

class TreeMerger {
public:
    TreeMerger(const params_t& params, result_t* result)
            : params_(params), result_(result) {
        init();
    }

    void init();

    void merge() {
        const int max_steps = 1000;
        for (int i = 0; i < max_steps; i++) {
            bool has_progress = merge_step();
            if (!has_progress) {
                break;
            }
        }
        result_->trees.clear();
        for (const auto& g : grid_) {
            for (const auto& priv_ti : g) {
                result_->trees.push_back(priv_ti.ti);
            }
        }
    }

    // return: true if has progress, false if no progress
    bool merge_step();
    
    int grid_id(double x, double y) {
        int x1 = ::floor(x / grid_size_);
        int y1 = ::floor(y / grid_size_);
        return y1 * grid_cols_ + x1;
    }

private:
    const params_t& params_;
    result_t* result_;
    int grid_size_;
    int grid_cols_;
    int grid_rows_;
    int grid_count_;
    vector<vector<private_tree_info_t>> grid_;
};

void TreeMerger::init() {
    LOG(INFO) << "max_tree_radius=" << params_.max_tree_radius;
    grid_size_ = params_.max_tree_radius * 1.2 + 1;
    LOG(INFO) << "choosing grid_size=" << grid_size_;
    grid_cols_ = params_.img_width / grid_size_;
    if (grid_cols_ * grid_size_ < params_.img_width) {
        grid_cols_++;
    }
    grid_rows_ = params_.img_height / grid_size_;
    if (grid_rows_ * grid_size_ < params_.img_height) {
        grid_rows_++;
    }
    LOG(INFO) << "image is cut into " << grid_cols_ << "x" << grid_rows_ << " grids";
    int bottom_right_grid_id = grid_id(params_.img_width - 0.0001, params_.img_height - 0.0001);
    grid_count_ = grid_cols_ * grid_rows_;
    CHECK_EQ(bottom_right_grid_id + 1, grid_count_);
    grid_.resize(grid_count_);
    
    for (const auto& t : result_->trees) {
        private_tree_info_t priv_ti;
        priv_ti.ti = t;
        // TODO load features (histogram, stddev of colors etc)
        int g = grid_id(t.x_pixels, t.y_pixels);
        grid_[g].push_back(priv_ti);
    }
    // for (size_t i = 0; i < grid_.size(); i++) {
    //     LOG(INFO) << "grid " << i << " has " << grid_[i].size() << " trees";
    // }
}

bool TreeMerger::merge_step() {
    // TODO merge trees!
    LOG(ERROR) << "NOT IMPLEMENTED YET: tree merge";
    for (int ga = 0; ga < grid_count_; ga++) {
        // only need to check neighbor grids on forward direction (right, down)
        int neighbor_grid_ids[5] = {ga, -1, -1, -1, -1};
        int neighbor_grid_count = 1;

        int ga_row = ga / grid_cols_;
        int ga_col = ga % grid_cols_;
        if (ga_col != grid_cols_ - 1) {
            neighbor_grid_ids[neighbor_grid_count] = ga + 1;
            neighbor_grid_count++;
        }
        if (ga_row < grid_rows_ - 1) {
            if (ga_col > 0) {
                neighbor_grid_ids[neighbor_grid_count] = ga + grid_cols_ - 1;
                neighbor_grid_count++;
            }
            neighbor_grid_ids[neighbor_grid_count] = ga + grid_cols_;
            neighbor_grid_count++;
            if (ga_col != grid_cols_ - 1) {
                neighbor_grid_ids[neighbor_grid_count] = ga + grid_cols_ + 1;
                neighbor_grid_count++;
            }
        }

        for (const auto& ta : grid_[ga]) {
            for (int idx = 0; idx < neighbor_grid_count; idx++) {
                int neighbor_grid_id = neighbor_grid_ids[idx];
                for (const auto& tb : grid_[neighbor_grid_id]) {
                    if (&ta == &tb) {
                        continue;
                    }

                    double dx = ta.ti.x_pixels - tb.ti.x_pixels;
                    double dy = ta.ti.y_pixels - tb.ti.y_pixels;
                    double dist_center_to_center = sqrt(dx*dx + dy*dy);
                    double dist_edge_to_edge = dist_center_to_center - ta.ti.radius_pixels - tb.ti.radius_pixels;
                    // LOG(INFO) << "dist_edge_to_edge=" << dist_edge_to_edge;
                    // TODO MERGERS!
                }
            }
        }
    }
    return false;
}

}  // anonymous namespace


namespace find_trees {

int find(const params_t& params, result_t* result) {
    CHECK_GT(params.img_width, 0);
    CHECK_GT(params.img_height, 0);
    CHECK_GT(params.max_tree_radius, 0);

    int ret = find_using_7x7_rgb_1(params, result);

    TreeMerger merger(params, result);
    merger.merge();

    return ret;
}

}  // namespace find_trees
