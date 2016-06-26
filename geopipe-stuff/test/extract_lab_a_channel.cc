#include "find_trees/colors.h"

#include <stdio.h>

#include <opencv2/opencv.hpp>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s input-image output-lab_a_channel-image\n", argv[0]);
        return 1;
    }

    cv::Mat input_image = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
    const int image_width = input_image.size().width;
    const int image_height = input_image.size().height;
    const int image_elem_size = input_image.elemSize();
    IplImage input_image_ipl = (IplImage) input_image;
    char* input_image_data = input_image_ipl.imageData;
    const int input_image_width_step = input_image_ipl.widthStep;
    const int image_pixels = image_width * image_height;
    uint8_t* contiguous_red = new uint8_t[image_pixels];
    uint8_t* contiguous_green = new uint8_t[image_pixels];
    uint8_t* contiguous_blue = new uint8_t[image_pixels];
    uint8_t* contiguous_lab_a = new uint8_t[image_pixels];

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

    find_trees::fill_lab_a_channel(image_pixels, contiguous_lab_a, contiguous_red, contiguous_green, contiguous_blue);

    for (int y = 0; y < image_height; y++) {
        int offst = y * image_width;
        int offst2 = y * input_image_width_step;
        for (int x = 0, x2 = 0; x < image_width; x++, x2 += image_elem_size) {
            // NOTE: opencv loaded image's channels are BGR, not RGB
            input_image_data[offst2 + x2] = contiguous_lab_a[offst + x];
            input_image_data[offst2 + x2 + 1] = contiguous_lab_a[offst + x];
            input_image_data[offst2 + x2 + 2] = contiguous_lab_a[offst + x];
        }
    }

    delete[] contiguous_red;
    delete[] contiguous_green;
    delete[] contiguous_blue;
    delete[] contiguous_lab_a;

    cv::imwrite(argv[2], input_image);

    return 0;
}
