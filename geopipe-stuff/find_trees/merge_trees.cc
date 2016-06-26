#include "merge_trees.h"
#include "find_trees.h"
#include "xxhash.h"

#include <functional>
#include <unordered_set>
#include <utility>
#include <vector>

#include <math.h>
#include <float.h>

#include <glog/logging.h>

namespace std {

template <>
struct hash<find_trees::tree_info_t> {
    size_t operator()(const find_trees::tree_info_t& ti) const {
        static int seed = 2016;
        return ::XXH32(&ti, sizeof(ti), seed);
    }
};

}  // namespace std


namespace {

using std::unordered_set;
using std::pair;
using std::vector;
using namespace find_trees;


static inline double distance2d(double x1, double y1, double x2, double y2) {
    double dx = x1 - x2;
    double dy = y1 - y2;
    return ::sqrt(dx*dx + dy*dy);
}

struct private_tree_info_t {
    tree_info_t ti;
    double cost_to_merge_with_nearest_tree;
    int nearest_tree_grid;
    int nearest_tree_idx_in_grid;
    bool covered_by_other_tree;

    int stat_signature = -1;
    int stat_tree_pixel_count = -1;
    int stat_not_tree_pixel_count = -1;
    double stat_intensity_avg = -1.0;
    double stat_intensity_var = -1.0;
    double stat_tree_pixel_center_x = -1.0;
    double stat_tree_pixel_center_y = -1.0;
};


class TreeMerger {
public:
    TreeMerger(const params_t& params, result_t* result)
            : params_(params), result_(result), tree_mask_(nullptr) {
        init();
    }
    ~TreeMerger() {
        delete[] tree_mask_;
    }

    void init();

    void merge() {
        const int max_steps = 1000;
        int i_step = 0;
        for (; i_step < max_steps; i_step++) {
            bool has_progress = merge_step();
            if (!has_progress) {
                break;
            }
        }

        // final greedy pass to remove smaller trees covered by big trees
        // sort trees by radius, big trees first. if a new tree has been covered by existing trees, remove it
        finalize_remove_occluded_trees();

        // prepare the results
        LOG(INFO) << "Merged trees in " << i_step << " steps";
        result_->trees.clear();
        for (const auto& g : grid_) {
            for (const auto& priv_ti : g) {
                result_->trees.push_back(priv_ti.ti);
            }
        }

        // remove the tiny small trees
        finalize_filter_noisy_tiny_trees();
    }

    // return: true if has progress, false if no progress
    bool merge_step();

    int grid_id(double x, double y) {
        int x1 = ::floor(x / grid_size_);
        int y1 = ::floor(y / grid_size_);
        return y1 * grid_cols_ + x1;
    }

private:
    void apply_on_near_by_tree_pairs(
        std::function<void(int /* ta_grid */, int /* ta_idx_in_grid */, private_tree_info_t& /* ta */,
                           int /* tb_grid */, int /* tb_idx_in_grid */, private_tree_info_t& /* tb */)>&& func);

    tree_info_t merge_trees(const tree_info_t& t1, const tree_info_t& t2);
    bool should_merge(const private_tree_info_t& t1, const private_tree_info_t& t2);

    double calc_cost_to_merge(const private_tree_info_t& ta, const private_tree_info_t& tb);

    void finalize_remove_occluded_trees();
    void finalize_filter_noisy_tiny_trees();

    // load features (histogram, stddev of colors etc)
    void update_tree_stat(private_tree_info_t* priv_tree);

    const params_t& params_;
    result_t* result_;
    int grid_size_;
    int grid_cols_;
    int grid_rows_;
    int grid_count_;
    vector<vector<private_tree_info_t>> grid_;

    // tree_mask_[pixel_id] == 0 means not tree, =1 means tree
    int8_t* tree_mask_;
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

    const int input_image_pixels = params_.img_width * params_.img_height;
    tree_mask_ = new int8_t[input_image_pixels];
    ::memset(tree_mask_, 0, input_image_pixels);

    for (auto tree_tile_id : result_->tree_tiles) {
        const int tree_row = tree_tile_id / result_->tile_cols;
        const int tree_col = tree_tile_id % result_->tile_cols;
        for (int y = tree_row * result_->tile_size; y < params_.img_height && y < tree_row * result_->tile_size + result_->tile_size ; y++) {
            int yoffst = y * params_.img_width;
            for (int x = tree_col * result_->tile_size; x < params_.img_width && x < tree_col * result_->tile_size + result_->tile_size ; x++) {
                tree_mask_[yoffst + x] = 1;
            }
        }
    }

    for (const auto& t : result_->trees) {
        private_tree_info_t priv_ti;
        priv_ti.ti = t;
        // load features (histogram, stddev of colors etc)
        update_tree_stat(&priv_ti);
        int g = grid_id(t.x_pixels, t.y_pixels);
        grid_[g].push_back(priv_ti);
    }
    // for (size_t i = 0; i < grid_.size(); i++) {
    //     LOG(INFO) << "grid " << i << " has " << grid_[i].size() << " trees";
    // }
}

void TreeMerger::apply_on_near_by_tree_pairs(
    std::function<void(int /* ta_grid */, int /* ta_idx_in_grid */, private_tree_info_t& /* ta */,
                       int /* tb_grid */, int /* tb_idx_in_grid */, private_tree_info_t& /* tb */)>&& func) {

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

        for (size_t idx_a = 0; idx_a < grid_[ga].size(); idx_a++) {
            auto& ta = grid_[ga][idx_a];
            for (int idx_neighbor = 0; idx_neighbor < neighbor_grid_count; idx_neighbor++) {
                int gb = neighbor_grid_ids[idx_neighbor];
                size_t idx_b = 0;
                if (ga == gb) {
                    idx_b = idx_a + 1;
                }
                for (; idx_b < grid_[gb].size(); idx_b++) {
                    auto& tb = grid_[gb][idx_b];
                    CHECK(&ta != &tb);

                    func(ga, idx_a, ta, gb, idx_b, tb);
                }
            }
        }
    }
}

tree_info_t TreeMerger::merge_trees(const tree_info_t& t1, const tree_info_t& t2) {
    tree_info_t merged;
    double dx = t1.x_pixels - t2.x_pixels;
    double dy = t1.y_pixels - t2.y_pixels;
    double d = ::sqrt(dx*dx + dy*dy);
    if (d < 0.001) {
        // merge into to bigger tree
        if (t1.radius_pixels < t2.radius_pixels) {
            return t2;
        } else {
            return t1;
        }
    }

    const tree_info_t* ta;  // point to smaller axis value
    const tree_info_t* tb;  // point to bigger axis value
    if (t2.x_pixels > t1.x_pixels) {
        ta = &t1;
        tb = &t2;
    } else {
        ta = &t2;
        tb = &t1;
    }

    double kx = (tb->x_pixels - ta->x_pixels) / d;
    double xl = ta->x_pixels - kx * ta->radius_pixels;
    double xr = tb->x_pixels + kx * tb->radius_pixels;
    double new_x = (xl + xr) / 2.0;
    // printf("d=%lf kx=%lf xl=%lf xr=%lf new_x=%lf\n", d, kx, xl, xr, new_x);

    if (t2.y_pixels > t1.y_pixels) {
        ta = &t1;
        tb = &t2;
    } else {
        ta = &t2;
        tb = &t1;
    }
    double ky = (tb->y_pixels - ta->y_pixels) / d;
    double yl = ta->y_pixels - ky * ta->radius_pixels;
    double yr = tb->y_pixels + ky * tb->radius_pixels;
    double new_y = (yl + yr) / 2.0;
    // printf("d=%lf ky=%lf yl=%lf yr=%lf new_y=%lf\n", d, ky, yl, yr, new_y);

    dx = xr - xl;
    dy = yr - yl;
    double new_r = 0.5 * ::sqrt(dx*dx + dy*dy);

    merged.x_pixels = new_x;
    merged.y_pixels = new_y;
    merged.radius_pixels = new_r;

    // printf("  *** merge: (%lf,%lf,r=%lf) & (%lf, %lf,r=%lf) => (%lf, %lf,r=%lf)\n",
    //        t1.x_pixels, t1.y_pixels, t1.radius_pixels, t2.x_pixels, t2.y_pixels, t2.radius_pixels,
    //        new_x, new_y, new_r);

    return merged;
}

bool TreeMerger::should_merge(const private_tree_info_t& t1, const private_tree_info_t& t2) {
    // check if can really merge (not exceed max tree radius, distance cannot be too big, not much empty pixels etc)

    double dx = t1.ti.x_pixels - t2.ti.x_pixels;
    double dy = t1.ti.y_pixels - t2.ti.y_pixels;
    double dist_center_to_center = sqrt(dx*dx + dy*dy);
    double dist_edge_to_edge = dist_center_to_center - t1.ti.radius_pixels - t2.ti.radius_pixels;
    if (dist_edge_to_edge < 0) {
        dist_edge_to_edge = 0;
    }

    // trees are too far apart
    double min_r = std::min(t1.ti.radius_pixels, t2.ti.radius_pixels);
    if (dist_edge_to_edge > 0.2 * min_r) {
        return false;
    }

    // tree is too big
    private_tree_info_t merged_ti;
    merged_ti.ti = merge_trees(t1.ti, t2.ti);
    if (merged_ti.ti.radius_pixels > params_.max_tree_radius) {
        return false;
    }

    const private_tree_info_t* t_small;
    const private_tree_info_t* t_big;
    if (t1.ti.radius_pixels < t2.ti.radius_pixels) {
        t_small = &t1;
        t_big = &t2;
    } else {
        t_small = &t2;
        t_big = &t1;
    }

    // if (dist_edge_to_edge < 1 && t_small->ti.radius_pixels < 0.2 * t_big->ti.radius_pixels) {
    //     return true;
    // }

    double not_tree_ratio1 = double(t1.stat_not_tree_pixel_count) / (t1.stat_tree_pixel_count + t1.stat_not_tree_pixel_count);
    double not_tree_ratio2 = double(t2.stat_not_tree_pixel_count) / (t2.stat_tree_pixel_count + t2.stat_not_tree_pixel_count);
    update_tree_stat(&merged_ti);
    double not_tree_ratio_merged = double(merged_ti.stat_not_tree_pixel_count) / (merged_ti.stat_tree_pixel_count + merged_ti.stat_not_tree_pixel_count);
    if (not_tree_ratio_merged > std::max(not_tree_ratio1, not_tree_ratio2) + 0.1 || not_tree_ratio_merged > 0.5) {
        // too many not-tree pixels considered part of a tree
        // LOG(INFO) << "ratio: " << not_tree_ratio1 << "  " << not_tree_ratio2 << " => " << not_tree_ratio_merged;
        return false;
    }

    return true;
}

bool TreeMerger::merge_step() {
    bool has_progress = false;

    for (auto& g : grid_) {
        for (auto& t : g) {
            t.cost_to_merge_with_nearest_tree = DBL_MAX;
            t.nearest_tree_grid = -1;
            t.nearest_tree_idx_in_grid = -1;
            update_tree_stat(&t);
            // move according to gravity center
            if (t.stat_tree_pixel_center_x >= 0.0 && t.stat_tree_pixel_center_y >= 0.0) {
                double d = distance2d(t.ti.x_pixels, t.ti.y_pixels, t.stat_tree_pixel_center_x, t.stat_tree_pixel_center_y);
                if (d > 0.49) {
                    // printf("XX %lf %lf %lf %lf d=%lf\n", t.ti.x_pixels, t.ti.y_pixels, t.stat_tree_pixel_center_x, t.stat_tree_pixel_center_y, d);
                    double new_x_pixels = (t.ti.x_pixels + t.stat_tree_pixel_center_x) / 2;
                    double new_y_pixels = (t.ti.y_pixels + t.stat_tree_pixel_center_y) / 2;
                    if (new_x_pixels >= 0 && new_x_pixels < params_.img_width && new_y_pixels >= 0 && new_y_pixels < params_.img_height) {
                        t.ti.x_pixels = new_x_pixels;
                        t.ti.y_pixels = new_y_pixels;
                        t.ti.radius_pixels -= 0.3 * d;
                        if (t.ti.radius_pixels < 0.1) {
                            t.ti.radius_pixels = 0.1;
                        }
                    }
                }
            }
        }
    }

    apply_on_near_by_tree_pairs([this] (int ta_grid, int ta_idx_in_grid, private_tree_info_t& ta,
                                    int tb_grid, int tb_idx_in_grid, private_tree_info_t& tb) {

        const double cost_to_merge = this->calc_cost_to_merge(ta, tb);
        if (cost_to_merge < ta.cost_to_merge_with_nearest_tree) {
            ta.cost_to_merge_with_nearest_tree = cost_to_merge;
            ta.nearest_tree_grid = tb_grid;
            ta.nearest_tree_idx_in_grid = tb_idx_in_grid;
            tb.cost_to_merge_with_nearest_tree = cost_to_merge;
            tb.nearest_tree_grid = ta_grid;
            tb.nearest_tree_idx_in_grid = ta_idx_in_grid;
        }
    });

    // for (auto& g : grid_) {
    //     for (auto& t : g) {
    //         if (t.nearest_tree_grid < 0 || t.nearest_tree_idx_in_grid < 0) {
    //             LOG(INFO) << "Tree x=" << t.ti.x_pixels << ",y=" << t.ti.y_pixels << ",r=" << t.ti.radius_pixels
    //                       << " does not have near by trees";
    //             continue;
    //         }
    //         const auto& t2 = grid_[t.nearest_tree_grid][t.nearest_tree_idx_in_grid];
    //         LOG(INFO) << "Tree x=" << t.ti.x_pixels << ",y=" << t.ti.y_pixels << ",r=" << t.ti.radius_pixels
    //                   << " closest to Tree x=" << t2.ti.x_pixels << ",y=" << t2.ti.y_pixels << ",r=" << t2.ti.radius_pixels
    //                   << "; dist=" << t.nearest_tree_dist_edge_to_edge;
    //     }
    // }

    unordered_set<tree_info_t> trees_merged;

    // will contain all trees in grid_ that are not merged
    vector<vector<private_tree_info_t>> new_grid;
    new_grid.resize(grid_count_);

    for (size_t i_grid = 0; i_grid < grid_.size(); i_grid++) {
        const auto& g = grid_[i_grid];
        for (const auto& t : g) {
            if (trees_merged.find(t.ti) != trees_merged.end()) {
                continue;
            }

            if (t.nearest_tree_grid < 0 || t.nearest_tree_idx_in_grid < 0) {
                // t has no near by trees, so keep t
                new_grid[i_grid].push_back(t);
                continue;
            }

            const auto& t2 = grid_[t.nearest_tree_grid][t.nearest_tree_idx_in_grid];
            if (trees_merged.find(t2.ti) != trees_merged.end()) {
                // t's nearest tree has been merged with others, so t is kept unmerged
                new_grid[i_grid].push_back(t);
                continue;
            }

            if (!should_merge(t, t2)) {
                // if cannot merge, keep t untouch, continue
                new_grid[i_grid].push_back(t);
                continue;
            }

            // merge t and t2, push new tree back to new_grid
            tree_info_t merged_ti = merge_trees(t.ti, t2.ti);
            private_tree_info_t priv_merged_ti;
            priv_merged_ti.ti = merged_ti;
            int merged_grid_id = grid_id(merged_ti.x_pixels, merged_ti.y_pixels);
            // LOG(INFO) << "GRID ID = " << merged_grid_id;
            new_grid[merged_grid_id].push_back(priv_merged_ti);
            trees_merged.insert(t.ti);
            trees_merged.insert(t2.ti);
            has_progress = true;
        }
    }
    grid_ = new_grid;


    // remove trees fully covered by another single tree

    for (auto& g : grid_) {
        for (auto& t : g) {
            t.covered_by_other_tree = false;
        }
    }

    // check if tree is fully covered by another tree
    apply_on_near_by_tree_pairs([&has_progress]
                                   (int ta_grid, int ta_idx_in_grid, private_tree_info_t& ta,
                                    int tb_grid, int tb_idx_in_grid, private_tree_info_t& tb) {

        private_tree_info_t* tbig;
        private_tree_info_t* tsmall;
        if (ta.ti.radius_pixels > tb.ti.radius_pixels) {
            tbig = &ta;
            tsmall = &tb;
        } else {
            tbig = &tb;
            tsmall = &ta;
        }
        double dx = ta.ti.x_pixels - tb.ti.x_pixels;
        double dy = ta.ti.y_pixels - tb.ti.y_pixels;
        double dist_center_to_center = sqrt(dx*dx + dy*dy);
        // tsmall->ti.radius_pixels - (tbig->ti.radius_pixels - dist_center_to_center):
        // the portion of small tree outside big tree
        double min_outside_ratio = 0.7;  // increase => less crowded trees (keep it < 1.0)
        if (tsmall->ti.radius_pixels - (tbig->ti.radius_pixels - dist_center_to_center) < min_outside_ratio * tsmall->ti.radius_pixels) {
            tsmall->covered_by_other_tree = true;
            has_progress = true;
        }
    });

    new_grid.clear();
    new_grid.resize(grid_count_);
    for (size_t i_grid = 0; i_grid < grid_.size(); i_grid++) {
        const auto& g = grid_[i_grid];
        for (const auto& t : g) {
            if (!t.covered_by_other_tree) {
                new_grid[i_grid].push_back(t);
            }
        }
    }
    grid_ = new_grid;

    return has_progress;
}

double TreeMerger::calc_cost_to_merge(const private_tree_info_t& ta, const private_tree_info_t& tb) {
    double dist_center_to_center = distance2d(ta.ti.x_pixels, ta.ti.y_pixels, tb.ti.x_pixels, tb.ti.y_pixels);
    double dist_border_to_border = dist_center_to_center - ta.ti.radius_pixels - tb.ti.radius_pixels;
    if (dist_border_to_border < 0) {
        dist_border_to_border = 0;
    }
    
    // derived from Bhattacharyya distance between intensity distribution
    // https://en.wikipedia.org/wiki/Bhattacharyya_distance
    double dist_intensity = 0;
    {
        double s1 = ta.stat_intensity_var * ta.stat_intensity_var;
        double s2 = tb.stat_intensity_var * tb.stat_intensity_var;
        double udiff = ta.stat_intensity_avg - ta.stat_intensity_avg;
        double udiff2 = udiff * udiff;
        // got rid of coefficient and some const, for faster calc
        dist_intensity = ::log(s1/s2 + s2/s1 + 2) + udiff2 / (s1 + s2);
    }


    // private_tree_info_t merged_ti;
    // merged_ti.ti = merge_trees(ta.ti, tb.ti);
    //
    // double not_tree_ratio1 = double(ta.stat_not_tree_pixel_count) / (ta.stat_tree_pixel_count + ta.stat_not_tree_pixel_count);
    // double not_tree_ratio2 = double(tb.stat_not_tree_pixel_count) / (tb.stat_tree_pixel_count + tb.stat_not_tree_pixel_count);
    // update_tree_stat(&merged_ti);
    // double not_tree_ratio_merged = double(merged_ti.stat_not_tree_pixel_count) / (merged_ti.stat_tree_pixel_count + merged_ti.stat_not_tree_pixel_count);

    // TODO maybe train some non-linear cost function?
    // LOG(INFO) << "dist inten=" << dist_intensity << " dis bord=" << dist_border_to_border;

    // dist_intensity usually in range 1.3 ~ 2.5
    // dist_border_to_border usually in range 30 ~ 120 (max tree size = 67, 7x7 tree tiles)
    double cost = 40.0 * dist_intensity + dist_border_to_border;
    return cost;
}

void TreeMerger::finalize_remove_occluded_trees() {
    size_t tree_count_before = 0;
    for (auto& g : grid_) {
        for (auto& t : g) {
            t.covered_by_other_tree = false;
            tree_count_before++;
        }
    }

    // TODO switch to pixel based code

    // similar to apply_on_near_by_tree_pairs, but check all 3x3 near by grids
    for (int ga = 0; ga < grid_count_; ga++) {
        int neighbor_grid_ids[9] = {ga, -1, -1, -1, -1};
        int neighbor_grid_count = 1;

        int ga_row = ga / grid_cols_;
        int ga_col = ga % grid_cols_;

        // 3 grids above ga
        if (ga_row > 0) {
            if (ga_col > 0) {
                neighbor_grid_ids[neighbor_grid_count] = ga - grid_cols_ - 1;
                neighbor_grid_count++;
            }
            neighbor_grid_ids[neighbor_grid_count] = ga - grid_cols_;
            neighbor_grid_count++;
            if (ga_col != grid_cols_ - 1) {
                neighbor_grid_ids[neighbor_grid_count] = ga - grid_cols_ + 1;
                neighbor_grid_count++;
            }
        }

        // left and right
        if (ga_col > 0) {
            neighbor_grid_ids[neighbor_grid_count] = ga - 1;
            neighbor_grid_count++;
        }
        if (ga_col != grid_cols_ - 1) {
            neighbor_grid_ids[neighbor_grid_count] = ga + 1;
            neighbor_grid_count++;
        }

        // 3 grids below ga
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

        for (size_t idx_a = 0; idx_a < grid_[ga].size(); idx_a++) {
            auto& ta = grid_[ga][idx_a];

            // use 5x5 boxes to cover the tree ta
            const int boxes_step = 5;
            // top-left point of boxes
            struct xy_pair {
                int x;
                int y;
            };
            vector<xy_pair> ta_boxes;
            for (int box_x = ta.ti.x_pixels - ta.ti.radius_pixels - boxes_step; box_x <= ta.ti.x_pixels + ta.ti.radius_pixels + boxes_step; box_x += boxes_step) {
                for (int box_y = ta.ti.y_pixels - ta.ti.radius_pixels - boxes_step; box_y <= ta.ti.y_pixels + ta.ti.radius_pixels + boxes_step; box_y += boxes_step) {
                    double d = distance2d(box_x, box_y, ta.ti.x_pixels, ta.ti.y_pixels);
                    if (d <= ta.ti.radius_pixels + 0.001) {
                        xy_pair xyp;
                        xyp.x = box_x;
                        xyp.y = box_y;
                        ta_boxes.push_back(xyp);
                        continue;
                    }
                    d = distance2d(box_x + boxes_step, box_y, ta.ti.x_pixels, ta.ti.y_pixels);
                    if (d <= ta.ti.radius_pixels + 0.001) {
                        xy_pair xyp;
                        xyp.x = box_x + boxes_step;
                        xyp.y = box_y;
                        ta_boxes.push_back(xyp);
                        continue;
                    }
                    d = distance2d(box_x, box_y + boxes_step, ta.ti.x_pixels, ta.ti.y_pixels);
                    if (d <= ta.ti.radius_pixels + 0.001) {
                        xy_pair xyp;
                        xyp.x = box_x;
                        xyp.y = box_y + boxes_step;
                        ta_boxes.push_back(xyp);
                        continue;
                    }
                    d = distance2d(box_x + boxes_step, box_y + boxes_step, ta.ti.x_pixels, ta.ti.y_pixels);
                    if (d <= ta.ti.radius_pixels + 0.001) {
                        xy_pair xyp;
                        xyp.x = box_x + boxes_step;
                        xyp.y = box_y + boxes_step;
                        ta_boxes.push_back(xyp);
                        continue;
                    }
                }
            }
            int orig_ta_boxes_count = ta_boxes.size();

            vector<private_tree_info_t*> tb_candidates;
            for (int idx_neighbor = 0; idx_neighbor < neighbor_grid_count; idx_neighbor++) {
                int gb = neighbor_grid_ids[idx_neighbor];
                for (size_t idx_b = 0; idx_b < grid_[gb].size(); idx_b++) {
                    auto& tb = grid_[gb][idx_b];
                    if (&tb == &ta) {
                        continue;
                    }
                    if (tb.ti.radius_pixels > ta.ti.radius_pixels) {
                        tb_candidates.push_back(&tb);
                    }
                }
            }
            // sort neighbor tb_candidates according to radius (bigger first), greedily check occulusion
            std::sort(tb_candidates.begin(), tb_candidates.end(),
            [] (private_tree_info_t* t1, private_tree_info_t* t2) {
                return t1->ti.radius_pixels > t2->ti.radius_pixels;
            });

            for (private_tree_info_t* tb : tb_candidates) {
                if (ta.covered_by_other_tree) {
                    break;
                }

                size_t i_box = 0;
                while (i_box < ta_boxes.size()) {
                    xy_pair xyp = ta_boxes[i_box];
                    double d = distance2d(xyp.x, xyp.y, tb->ti.x_pixels, tb->ti.y_pixels);
                    if (d > tb->ti.radius_pixels - 0.001) {
                        // box not covered
                        i_box++;
                        continue;
                    }
                    d = distance2d(xyp.x, xyp.y + boxes_step, tb->ti.x_pixels, tb->ti.y_pixels);
                    if (d > tb->ti.radius_pixels - 0.001) {
                        // box not covered
                        i_box++;
                        continue;
                    }
                    d = distance2d(xyp.x + boxes_step, xyp.y, tb->ti.x_pixels, tb->ti.y_pixels);
                    if (d > tb->ti.radius_pixels - 0.001) {
                        // box not covered
                        i_box++;
                        continue;
                    }
                    d = distance2d(xyp.x + boxes_step, xyp.y + boxes_step, tb->ti.x_pixels, tb->ti.y_pixels);
                    if (d > tb->ti.radius_pixels - 0.001) {
                        // box not covered
                        i_box++;
                        continue;
                    }

                    // remove covered ones from ta_boxes
                    ta_boxes[i_box] = ta_boxes.back();
                    ta_boxes.pop_back();
                    // no i_box++
                }

                // if nearly all of the tree has been covered
                if (ta_boxes.size() < 0.1 * orig_ta_boxes_count) {
                    ta.covered_by_other_tree = true;
                }
            }
        }
    }

    vector<vector<private_tree_info_t>> new_grid;
    new_grid.resize(grid_count_);
    size_t tree_count_after = 0;
    for (size_t i_grid = 0; i_grid < grid_.size(); i_grid++) {
        const auto& g = grid_[i_grid];
        for (const auto& t : g) {
            if (!t.covered_by_other_tree) {
                new_grid[i_grid].push_back(t);
                tree_count_after++;
            }
        }
    }
    grid_ = new_grid;

    LOG(INFO) << "Remove occuluded trees: " << tree_count_before << " => " << tree_count_after;
}

void TreeMerger::finalize_filter_noisy_tiny_trees() {
    if (result_->trees.size() < 10) {
        return;
    }
    vector<double> radius_dist;
    radius_dist.reserve(result_->trees.size());

    double radius_sum = 0.0;
    for (const auto& t : result_->trees) {
        radius_dist.push_back(t.radius_pixels);
        radius_sum += t.radius_pixels;
    }
    double radius_avg = radius_sum / result_->trees.size();

    std::sort(radius_dist.begin(), radius_dist.end());
    int filter_pct = 30;
    size_t idx = radius_dist.size() * filter_pct / 100;
    double radius_filter1 = radius_dist[idx];
    // LOG(INFO) << "radius min: " << radius_dist.front() << " max:" << radius_dist.back()
    //     << " avg:" << radius_avg << " " << filter_pct << "%: " << radius_filter1 << " radius_var =" << radius_var;
    double radius_filter2 = (radius_filter1 + radius_avg) / 2;
    LOG(INFO) << "Filtering trees with radius < " << radius_filter2;
    int before_cnt = result_->trees.size();
    idx = 0;
    while (idx < result_->trees.size()) {
        if (result_->trees[idx].radius_pixels < radius_filter2) {
            result_->trees[idx] = result_->trees.back();
            result_->trees.pop_back();
            continue;
        }
        idx++;
    }
    LOG(INFO) << "Filtered noisy small trees: " << before_cnt << " => " << result_->trees.size();
}

void TreeMerger::update_tree_stat(private_tree_info_t* priv_tree) {
    // load features (histogram, stddev of colors etc)
    size_t cksum = std::hash<tree_info_t>()(priv_tree->ti);
    int cksum2 = static_cast<int>(cksum & 0x7FFFFFFF);
    if (cksum2 == priv_tree->stat_signature) {
        // tree info not changed, do not need to update
        return;
    }
    priv_tree->stat_signature = cksum2;

    // based on mid-point algorithm
    // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
    const double x_center = priv_tree->ti.x_pixels;
    const double y_center = priv_tree->ti.y_pixels;
    const double radius = priv_tree->ti.radius_pixels;
    double xx = radius;
    double yy = 0;
    double err = 0;

    int num_tree_pixels = 0;
    int num_not_tree_pixels = 0;
    int num_pixels = 0;
    double sum_intensity = 0;
    double sum_intensity2 = 0;
    double sum_x = 0;
    double sum_y = 0;

    auto collect_stats = [&num_tree_pixels, &num_not_tree_pixels, &num_pixels, &sum_intensity, &sum_intensity2, this,
                          &sum_x, &sum_y]
    (const int y, int xl, int xr) {
        int yoffst = y * this->params_.img_width;
        if (xl < 0) {
            xl = 0;
        }
        if (xl >= this->params_.img_width) {
            xl = this->params_.img_width - 1;
        }
        if (xr < 0) {
            xr = 0;
        }
        if (xr >= this->params_.img_width) {
            xr = this->params_.img_width - 1;
        }

        num_pixels += xr - xl + 1;

        CHECK_GE(xr, xl);
        for (int x = xl; x <= xr; x++) {
            if (this->tree_mask_[yoffst + x]) {
                num_tree_pixels++;
                sum_x += x;
                sum_y += y;
            } else {
                num_not_tree_pixels++;
            }

            double intensity = 0.2126 * this->params_.channel_red[yoffst + x] +
                               0.7152 * this->params_.channel_green[yoffst + x] +
                               0.0722 * this->params_.channel_blue[yoffst + x];

            sum_intensity += intensity;
            sum_intensity2 += intensity * intensity;
        }
    };
    // iterate every pixel inside the tree, collect stats
    while (xx >= yy) {
        int y = y_center + yy;
        int xl = x_center - xx;
        int xr = x_center + xx;

        // for x in [xl..xr], do (x, y)...
        if (y >= 0 && y < params_.img_height) {
            collect_stats(y, xl, xr);
        }

        y = y_center - yy;
        // for x in [xl..xr], do (x, y)...
        if (y >= 0 && y < params_.img_height) {
            collect_stats(y, xl, xr);
        }

        y = y_center + xx;
        xl = x_center - yy;
        xr = x_center + yy;
        // for x in [xl..xr], do (x, y)...
        if (y >= 0 && y < params_.img_height) {
            collect_stats(y, xl, xr);
        }

        y = y_center - xx;
        // for x in [xl..xr], do (x, y)...
        if (y >= 0 && y < params_.img_height) {
            collect_stats(y, xl, xr);
        }

        yy++;
        err += 1 + 2*yy;
        if (2*(err-xx) + 1 > 0) {
            xx--;
            err += 1-2*xx;
        }
    }


    // LOG(INFO) << "tree " << num_tree_pixels << " not tree " << num_not_tree_pixels;
    priv_tree->stat_tree_pixel_count = num_tree_pixels;
    priv_tree->stat_not_tree_pixel_count = num_not_tree_pixels;

    // LOG(INFO) << "num pixels=" << num_pixels;
    double expectation1 = sum_intensity / num_pixels;
    double expectation2 = sum_intensity2 / num_pixels - expectation1*expectation1;
    priv_tree->stat_intensity_avg = expectation1;
    priv_tree->stat_intensity_var = ::sqrt(expectation2);
    // LOG(INFO) << "intensity: avg=" << priv_tree->stat_intensity_avg << " var=" << priv_tree->stat_intensity_var;
    if (num_tree_pixels > 0) {
        priv_tree->stat_tree_pixel_center_x = sum_x / num_tree_pixels;
        priv_tree->stat_tree_pixel_center_y = sum_y / num_tree_pixels;
    } else {
        priv_tree->stat_tree_pixel_center_x = priv_tree->ti.x_pixels;
        priv_tree->stat_tree_pixel_center_y = priv_tree->ti.y_pixels;
    }
    // printf("YYY: %lf %lf %lf %lf %lf\n", priv_tree->ti.x_pixels, priv_tree->ti.y_pixels, priv_tree->stat_tree_pixel_center_x, priv_tree->stat_tree_pixel_center_x, priv_tree->ti.radius_pixels);
}

}  // anonymous namespace


namespace find_trees {

int merge_trees(const params_t& params, result_t* result) {
    TreeMerger merger(params, result);
    merger.merge();
    return 0;
}

}  // namespace find_trees
