#include "utils/inc_gflags.h"
#include <stdio.h>
#include <glog/logging.h>
#include <laslib/lasreader.hpp>
#include <math.h>
#include <string.h>
#include <vector>

#include "find_trees/debugging.h"
#include "find_trees/las_stats.h"

using std::vector;
using namespace find_trees;

DEFINE_double(resolution, 0.15, "meters per pixel on output file");
DEFINE_double(radius_in_meters, 2, "how far does a las point affect");

int main(int argc, char* argv[]) {
    FLAGS_logtostderr = 1;
    google::InitGoogleLogging(argv[0]);
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    if (argc < 3) {
        printf("Usage: %s <las_file> <output_stats_file>\n", argv[0]);
        return 1;
    }

    LASreadOpener lasreadopener;
    std::string las_fn = argv[1];
    lasreadopener.set_file_name(las_fn.c_str());
    LASreader* lasreader = lasreadopener.open();

    const double min_x = lasreader->get_min_x();
    const double max_x = lasreader->get_max_x();
    const double min_y = lasreader->get_min_y();
    const double max_y = lasreader->get_max_y();
    const double min_z = lasreader->get_min_z();
    const double max_z = lasreader->get_max_z();

    LOG(INFO) << "x: " << min_x << " ~ " << max_x;
    LOG(INFO) << "y: " << min_y << " ~ " << max_y;
    LOG(INFO) << "z: " << min_z << " ~ " << max_z;

    const int img_width = ceilf((max_x - min_x) / FLAGS_resolution);
    const int img_height = ceilf((max_y - min_y) / FLAGS_resolution);
    const int img_pixels = img_width * img_height;
    LOG(INFO) << "image size: " << img_width << " x " << img_height;

    double* output_weight = new double[img_pixels];
    double* output_stddev_z = new double[img_pixels];
    for (int i = 0; i < img_pixels; i++) {
        output_weight[i] = 0.0;
        output_stddev_z[i] = 0.0;
    }
    uint8_t* img_gray = new uint8_t[img_pixels];
    memset(img_gray, 0, img_pixels);

    vector<point3d_t> all_points;
    all_points.reserve(lasreader->npoints);

    LOG(INFO) << "reading " << lasreader->npoints << " points";
    while (lasreader->read_point()) {
        const LASpoint& p = lasreader->point;
        point3d_t pt;
        pt.x = p.get_x();
        pt.y = p.get_y();
        pt.z = p.get_z();
        all_points.push_back(pt);

        int x_pixels = quantize(pt.x, min_x, max_x, img_width);
        int y_pixels = img_height - 1 - quantize(pt.y, min_y, max_y, img_height);

        img_gray[y_pixels * img_width + x_pixels] = 255;
    }
    lasreader->close();
    LOG(INFO) << "done with reading";

    LOG(INFO) << "write debug image: /tmp/las-debug-points.pgm";
    debug_write_pgm("/tmp/las-debug-points.pgm", img_width, img_height, img_gray);

    points_stats(&all_points[0], all_points.size(), min_x, max_x, min_y, max_y,
                 FLAGS_resolution, FLAGS_radius_in_meters, output_weight, output_stddev_z);

    for (int i = 0; i < img_pixels; i++) {
        img_gray[i] = output_weight[i];
    }

    LOG(INFO) << "write debug image: /tmp/las-debug-weight.pgm";
    debug_write_pgm("/tmp/las-debug-weight.pgm", img_width, img_height, img_gray);

    return 0;
}
