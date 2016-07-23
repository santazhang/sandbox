#include "utils/inc_gflags.h"
#include "find_trees.h"

#include <iostream>

#include <string.h>
#include <stdio.h>

#include <laslib/lasreader.hpp>
#include <glog/logging.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace find_trees;

DEFINE_string(ir, "", "IR image");
DEFINE_string(las, "", "LAS file");
DEFINE_double(resolution, 0.15, "meters per pixel on output file");

int main(int argc, char* argv[]) {
    FLAGS_logtostderr = 1;
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    ::google::InitGoogleLogging(argv[0]);

    if (argc < 2) {
        printf("Usage: %s <image-file> [--ir=ir_image] [--las=las_file]\n", argv[0]);
        exit(1);
    }

    LOG(WARNING) << "NOTE: currently (2016.07) only supports 0.15m resolution";
    LOG(INFO) << "Using resolution = " << FLAGS_resolution;

    cv::Mat input_image = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
    //cv::imshow("Input image", input_image);
    //cv::waitKey(0);

    const int image_width = input_image.size().width;
    const int image_height = input_image.size().height;
    const int image_channels = input_image.channels();
    const int image_elem_size = input_image.elemSize();
    CHECK_EQ(image_channels, 3);
    LOG(INFO) << "Image element size: " << image_elem_size;
    LOG(INFO) << "Image width and height: " << image_width << " x " << image_height;

    // int y = 231;
    // int x = 434;
    // cv::Vec3b intensity = input_image.at<cv::Vec3b>(y, x);
    // // NOTE: opencv loaded image's channels are BGR, not RGB
    // uchar blue = intensity.val[0];
    // uchar green = intensity.val[1];
    // uchar red = intensity.val[2];
    // LOG(INFO) << "BGR=" << int(blue) << ", " << int(green) << ", " << int(red);

    IplImage input_image_ipl = (IplImage) input_image;
    const char* input_image_data = input_image_ipl.imageData;
    const int input_image_width_step = input_image_ipl.widthStep;
    // int blue2 = (uchar) input_image_data[input_image_width_step * y + x * input_image.elemSize()];
    // int green2 = (uchar) input_image_data[input_image_width_step * y + x * input_image.elemSize() + 1];
    // int red2 = (uchar) input_image_data[input_image_width_step * y + x * input_image.elemSize() + 2];
    // LOG(INFO) << "BGR2=" << int(blue2) << ", " << int(green2) << ", " << int(red2);

    const int image_pixels = image_width * image_height;
    uint8_t* contiguous_red = new uint8_t[image_pixels];
    uint8_t* contiguous_green = new uint8_t[image_pixels];
    uint8_t* contiguous_blue = new uint8_t[image_pixels];
    // memset(contiguous_red, 0, image_pixels);
    // memset(contiguous_green, 0, image_pixels);
    // memset(contiguous_blue, 0, image_pixels);
    
    for (int y = 0; y < image_height; y++) {
        int offst = y * image_width;
        int offst2 = y * input_image_width_step;
        for (int x = 0, x2 = 0; x < image_width; x++, x2 += image_elem_size) {
            // NOTE: opencv loaded image's channels are BGR, not RGB
            contiguous_red[offst + x] = input_image_data[offst2 + x2 + 2];
            contiguous_green[offst + x] = input_image_data[offst2 + x2 + 1];
            contiguous_blue[offst + x] = input_image_data[offst2 + x2];
        }
    }

    uint8_t* contiguous_ir = nullptr;
    if (FLAGS_ir != "") {
        cv::Mat ir_image = cv::imread(FLAGS_ir, CV_LOAD_IMAGE_GRAYSCALE);
        CHECK_EQ(ir_image.channels(), 1);
        CHECK_EQ(ir_image.size().width, image_width);
        CHECK_EQ(ir_image.size().height, image_height);
        IplImage ir_image_ipl = (IplImage) ir_image;
        const char* ir_image_data = ir_image_ipl.imageData;
        const int ir_image_width_step = ir_image_ipl.widthStep;
        contiguous_ir = new uint8_t[image_pixels];
        for (int y = 0; y < image_height; y++) {
            int offst = y * image_width;
            int offst2 = y * ir_image_width_step;
            for (int x = 0, x2 = 0; x < image_width; x++, x2 += image_elem_size) {
                contiguous_ir[offst + x] = ir_image_data[offst2 + x2];
            }
        }
    }
    
    find_trees::params_t params;

    vector<point3d_t> all_points;

    if (FLAGS_las != "") {
        LASreadOpener lasreadopener;
        std::string las_fn = FLAGS_las;
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

        const int las_img_width = ceilf((max_x - min_x) / FLAGS_resolution);
        const int las_img_height = ceilf((max_y - min_y) / FLAGS_resolution);
        const int las_img_pixels = las_img_width * las_img_height;
        LOG(INFO) << "las image size: " << las_img_width << " x " << las_img_height;

        all_points.reserve(lasreader->npoints);

        LOG(INFO) << "reading " << lasreader->npoints << " points";
        while (lasreader->read_point()) {
            const LASpoint& p = lasreader->point;
            point3d_t pt;
            pt.x = p.get_x();
            pt.y = p.get_y();
            pt.z = p.get_z();
            all_points.push_back(pt);
        }
        lasreader->close();
        LOG(INFO) << "done with reading";

        params.n_points = all_points.size();
        params.points = &all_points[0];
        params.points_resolution = FLAGS_resolution;
        params.points_min_x = min_x;
        params.points_max_x = max_x;
        params.points_min_y = min_y;
        params.points_max_y = max_y;
    }

    params.img_width = image_width;
    params.img_height = image_height;
    params.channel_red = contiguous_red;
    params.channel_green = contiguous_green;
    params.channel_blue = contiguous_blue;
    params.channel_ir = contiguous_ir;

    params.max_tree_radius = ceilf(10.0 / FLAGS_resolution);

    find_trees::result_t result;
    int st = find_trees::find(params, &result);
    LOG(INFO) << "Find return code = " << st;
    LOG(INFO) << "Found " << result.trees.size() << " trees";
    LOG(INFO) << "Found " << result.tree_tiles.size() << " tree tiles";

    cv::Mat tree_tiles_overlay;
    input_image.copyTo(tree_tiles_overlay);
    for (const auto& t : result.tree_tiles) {
        int row = t / result.tile_cols;
        int col = t % result.tile_cols;
        cv::Point p1(col * result.tile_step_x, row * result.tile_step_y);
        cv::Point p2(p1.x + result.tile_size, p1.y + result.tile_size);
        cv::rectangle(tree_tiles_overlay, p1, p2, cv::Scalar(0, 255, 0));
    }
    // http://stackoverflow.com/a/36005085
    const double alpha = 0.2;
    cv::addWeighted(tree_tiles_overlay, alpha, input_image, 1 - alpha, 0, input_image);
    for (const auto& t : result.trees) {
        cv::Point pt(t.x_pixels, t.y_pixels);
        cv::circle(input_image, pt, t.radius_pixels, cv::Scalar(0, 0, 255) /* red */);
    }

    // cv::imshow("Trees", input_image);
    // cv::waitKey(0);

    const char* result_img_fn = "/tmp/find-trees-result.png";
    cv::imwrite(result_img_fn, input_image);
    LOG(INFO) << "Result image saved to: " << result_img_fn;

    delete[] contiguous_red;
    delete[] contiguous_green;
    delete[] contiguous_blue;

    if (contiguous_ir != nullptr) {
        delete[] contiguous_ir;
    }

    return 0;
}
