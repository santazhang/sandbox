#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#include <Cocoa/Cocoa.h>

#include "common.h"


OSStatus GenerateThumbnailForURL(void *thisInterface,
                                 QLThumbnailRequestRef thumbnail,
                                 CFURLRef url,
                                 CFStringRef contentTypeUTI,
                                 CFDictionaryRef options,
                                 CGSize maxSize);
void CancelThumbnailGeneration(void *thisInterface,
                               QLThumbnailRequestRef thumbnail);


CGImageRef PSPISOThumbnail(CFStringRef fpath) {
    char* fpath_cstr = MYCFStringCopyUTF8String(fpath);
    int datalen = -1;
    char* pngdata = isofetch(fpath_cstr, "PSP_GAME/ICON0.PNG", &datalen);
    free(fpath_cstr);

    if (pngdata == NULL) {
        return NULL;
    }

    NSData* nsdata = [NSData dataWithBytesNoCopy:pngdata
                                          length:datalen
                                    freeWhenDone:YES];
    CGDataProviderRef imgdata_provider =
        CGDataProviderCreateWithCFData((__bridge CFDataRef) nsdata);
    CGImageRef image =
        CGImageCreateWithPNGDataProvider(imgdata_provider,
                                         NULL,
                                         true,
                                         kCGRenderingIntentDefault);

    return image;
}


CGImageRef PBPThumbnail(CFStringRef fpath) {
    char* fpath_cstr = MYCFStringCopyUTF8String(fpath);
    FILE* fp = fopen(fpath_cstr, "rb");
    free(fpath_cstr);

    char magic[4];
    fread(magic, 1, 4, fp);

    int32_t offst;

    fseek(fp, 12, SEEK_SET);
    fread(&offst, sizeof(int32_t), 1, fp);
    int icon0_offst = offst;

    fseek(fp, 16, SEEK_SET);
    fread(&offst, sizeof(int32_t), 1, fp);
    int icon1_offst = offst;

    if (icon1_offst - icon0_offst > 0) {
        int size = icon1_offst - icon0_offst;
        fseek(fp, icon0_offst, SEEK_SET);
        char* pngdata = (char*) malloc(size);
        fread(pngdata, size, 1, fp);

        NSData * nsdata = [NSData dataWithBytesNoCopy:pngdata
                                               length:size
                                         freeWhenDone:YES];
        CGDataProviderRef imgdata_provider =
            CGDataProviderCreateWithCFData((__bridge CFDataRef) nsdata);
        CGImageRef image =
            CGImageCreateWithPNGDataProvider(imgdata_provider,
                                             NULL,
                                             true,
                                             kCGRenderingIntentDefault);

        return image;
    }
    fclose(fp);

    return NULL;
}


// http://dsibrew.org/wiki/NDS_Format
// https://code.google.com/p/tinke/source/browse/trunk/Tinke/Nitro/NDS.cs
CGImageRef NDSThumbnail(CFStringRef fpath) {
    char* fpath_cstr = MYCFStringCopyUTF8String(fpath);
    FILE* fp = fopen(fpath_cstr, "rb");
    free(fpath_cstr);

    if (fp == NULL) {
        return NULL;
    }

    NSBitmapImageRep* bmp = [[NSBitmapImageRep alloc]
                             initWithBitmapDataPlanes:NULL
                             pixelsWide:32
                             pixelsHigh:32
                             bitsPerSample:8
                             samplesPerPixel:3  // RGB, 3 channels
                             hasAlpha:NO
                             isPlanar:NO
                             colorSpaceName:NSDeviceRGBColorSpace
                             bytesPerRow:(32 * 3)
                             bitsPerPixel:24];

    if (bmp == NULL) {
        return NULL;
    }

    int32_t icon_title_offst;
    fseek(fp, 0x68, SEEK_SET);
    fread(&icon_title_offst, sizeof(int32_t), 1, fp);

    fseek(fp, icon_title_offst + 0x20, SEEK_SET);
    unsigned char icon[512];

    uint16_t palette[16];
    fread(icon, 1, 512, fp);
    fread(palette, sizeof(uint16_t), 16, fp);

    uint8* data = (uint8 *) bmp.bitmapData;

    for (int tile = 0; tile < 16; tile++) {
        int tile_row = tile / 4;
        int tile_col = tile % 4;

        for (int px = 0; px < 32; px++) {
            int px_row = (px * 2) / 8;
            int px_col = (px * 2) % 8;
            int px_l = icon[px + tile * 0x20] & 0xF;
            int px_r = (icon[px + tile * 0x20] >> 4) & 0xF;

            int x = tile_col * 8 + px_col;
            int y = tile_row * 8 + px_row;

            uint8* pxl = &data[y * 32 * 3 + x * 3];
            if (px_l != 0) {
                pxl[0] = (palette[px_l] & 0x1F) * 8;
                pxl[1] = ((palette[px_l] >> 5) & 0x1F) * 8;
                pxl[2] = ((palette[px_l] >> 10) & 0x1F) * 8;
            }

            pxl = &data[y * 32 * 3 + (x + 1) * 3];
            if (px_r != 0) {
                pxl[0] = (palette[px_r] & 0x1F) * 8;
                pxl[1] = ((palette[px_r] >> 5) & 0x1F) * 8;
                pxl[2] = ((palette[px_r] >> 10) & 0x1F) * 8;
            }
        }
    }
    fclose(fp);

    return [bmp CGImage];
}


OSStatus GenerateThumbnailForURL(void *thisInterface,
                                 QLThumbnailRequestRef thumbnail,
                                 CFURLRef url,
                                 CFStringRef contentTypeUTI,
                                 CFDictionaryRef options,
                                 CGSize maxSize) {
    CFStringRef fpath = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);

    if (QLThumbnailRequestIsCancelled(thumbnail)) {
        return noErr;
    }

    CGImageRef image = NULL;

    if (CFStringHasSuffix(fpath, CFSTR(".nds")) ||
        CFStringHasSuffix(fpath, CFSTR(".NDS"))) {
        image = NDSThumbnail(fpath);
    } else if (CFStringHasSuffix(fpath, CFSTR(".pbp")) ||
               CFStringHasSuffix(fpath, CFSTR(".PBP"))) {
        image = PBPThumbnail(fpath);
    } else if (CFStringHasSuffix(fpath, CFSTR(".iso")) ||
               CFStringHasSuffix(fpath, CFSTR(".ISO"))) {
        image = PSPISOThumbnail(fpath);
    }

    if (image == NULL || QLThumbnailRequestIsCancelled(thumbnail)) {
        return noErr;
    }

    size_t w = CGImageGetWidth(image);
    size_t h = CGImageGetHeight(image);

    CGSize size = CGSizeMake(w, h);
    CGRect rect = {{0, 0}, {size.width, size.height}};

    CGContextRef ctx = CreateARGBBitmapContext(size);
    CGContextDrawImage(ctx, rect, image);
    CGImageRef new_image = CGBitmapContextCreateImage(ctx);
    CGContextRelease(ctx);

    QLThumbnailRequestSetImage(thumbnail, new_image, NULL);
    CGImageRelease(new_image);

    return noErr;
}


void CancelThumbnailGeneration(void *thisInterface,
                               QLThumbnailRequestRef thumbnail) {
    // Implement only if supported
}
