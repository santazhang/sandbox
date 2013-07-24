#include "common.h"


char * MYCFStringCopyUTF8String(CFStringRef str) {
    if (str == NULL) {
        return NULL;
    }
    CFIndex len = CFStringGetLength(str);
    CFIndex max_size = CFStringGetMaximumSizeForEncoding(len, kCFStringEncodingUTF8);
    char * buf = (char *) malloc(max_size);
    if (CFStringGetCString(str, buf, max_size, kCFStringEncodingUTF8)) {
        return buf;
    }
    return NULL;
}


// https://github.com/lnxbil/quicklook-pfm/blob/master/src/GenerateThumbnailForURL.m
CGContextRef CreateARGBBitmapContext(CGSize size) {
    CGContextRef    context = NULL;
    CGColorSpaceRef colorSpace;
    void *          bitmapData;
    unsigned long   bitmapByteCount;
    unsigned long   bitmapBytesPerRow;

    // Get image width, height. We'll use the entire image.
    size_t pixelsWide = size.width;
    size_t pixelsHigh = size.height;

    // Declare the number of bytes per row. Each pixel in the bitmap in this
    // example is represented by 4 bytes; 8 bits each of red, green, blue, and
    // alpha.
    bitmapBytesPerRow   = pixelsWide * 4;
    bitmapByteCount     = bitmapBytesPerRow * pixelsHigh;

    // Use the generic RGB color space.
    colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
    if (colorSpace == NULL) {
        fprintf(stderr, "Error allocating color space\n");
        return NULL;
    }

    // Allocate memory for image data. This is the destination in memory
    // where any drawing to the bitmap context will be rendered.
    bitmapData = malloc(bitmapByteCount);
    if (bitmapData == NULL) {
        fprintf(stderr, "Memory not allocated!");
        CGColorSpaceRelease(colorSpace);
        return NULL;
    }

    // Create the bitmap context. We want pre-multiplied ARGB, 8-bits
    // per component. Regardless of what the source image format is
    // (CMYK, Grayscale, and so on) it will be converted over to the format
    // specified here by CGBitmapContextCreate.
    context = CGBitmapContextCreate(bitmapData,
                                    pixelsWide,
                                    pixelsHigh,
                                    8,  // bits per component
                                    bitmapBytesPerRow,
                                    colorSpace,
                                    kCGImageAlphaPremultipliedFirst);
    if (context == NULL) {
        free(bitmapData);
        fprintf(stderr, "Context not created!");
    }

    // Make sure and release colorspace before returning
    CGColorSpaceRelease(colorSpace);

    return context;
}
