#include "utils/inc_gflags.h"
#include "find_trees.h"

#include <iostream>

#include <string.h>
#include <stdio.h>

#include <glog/logging.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace find_trees;

int main(int argc, char* argv[]) {
    FLAGS_logtostderr = 1;
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    ::google::InitGoogleLogging(argv[0]);

    if (argc < 2) {
        printf("Usage: %s <image-file>\n", argv[0]);
        exit(1);
    }

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

    find_trees::params_t params;
    params.img_width = input_image.size().width;
    params.img_height = input_image.size().height;
    params.channel_red = contiguous_red;
    params.channel_green = contiguous_green;
    params.channel_blue = contiguous_blue;
    find_trees::result_t result;
    int st = find_trees::find(params, &result);
    LOG(INFO) << "Find return code = " << st;
    LOG(INFO) << "Found " << result.trees.size() << " trees";

    for (const auto& t : result.trees) {
        cv::Point pt(t.x_pixels, t.y_pixels);
        cv::circle(input_image, pt, t.radius_pixels, cv::Scalar(0, 0, 255) /* red */);
    }
    cv::imshow("Trees", input_image);
    cv::waitKey(0);

    delete[] contiguous_red;
    delete[] contiguous_green;
    delete[] contiguous_blue;

    return 0;
}
