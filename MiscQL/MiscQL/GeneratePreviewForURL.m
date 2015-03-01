#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#include <Cocoa/Cocoa.h>

#include "common.h"

OSStatus GeneratePreviewForURL(void *thisInterface,
                               QLPreviewRequestRef preview,
                               CFURLRef url,
                               CFStringRef contentTypeUTI,
                               CFDictionaryRef options);
void CancelPreviewGeneration(void *thisInterface, QLPreviewRequestRef preview);


CGImageRef PSPISOPreview(CFStringRef fpath) {
    char* fpath_cstr = MYCFStringCopyUTF8String(fpath);
    int datalen = -1;
    char* pngdata = isofetch(fpath_cstr, "PSP_GAME/PIC1.PNG", &datalen);
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


CGImageRef PBPPreview(CFStringRef fpath) {
    char* fpath_cstr = MYCFStringCopyUTF8String(fpath);
    FILE* fp = fopen(fpath_cstr, "rb");
    free(fpath_cstr);

    char magic[4];
    fread(magic, 1, 4, fp);

    int32_t offst;
    fseek(fp, 24, SEEK_SET);
    fread(&offst, sizeof(int32_t), 1, fp);
    int pic1_offst = offst;

    fseek(fp, 28, SEEK_SET);
    fread(&offst, sizeof(int32_t), 1, fp);
    int snd0_offst = offst;

    fseek(fp, 32, SEEK_SET);
    fread(&offst, sizeof(int32_t), 1, fp);

    fseek(fp, 36, SEEK_SET);
    fread(&offst, sizeof(int32_t), 1, fp);

    if (snd0_offst - pic1_offst > 0) {
        int size = snd0_offst - pic1_offst;
        fseek(fp, pic1_offst, SEEK_SET);
        char* pngdata = (char*) malloc(size);
        fread(pngdata, size, 1, fp);

        NSData* nsdata = [NSData dataWithBytesNoCopy:pngdata
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


OSStatus GeneratePreviewForURL(void *thisInterface,
                               QLPreviewRequestRef preview,
                               CFURLRef url,
                               CFStringRef contentTypeUTI,
                               CFDictionaryRef options) {
    CFStringRef fpath = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);

    if (QLPreviewRequestIsCancelled(preview)) {
        return noErr;
    }

    CGImageRef image = NULL;

    if (CFStringHasSuffix(fpath, CFSTR(".iso")) ||
        CFStringHasSuffix(fpath, CFSTR(".ISO"))) {
        image = PSPISOPreview(fpath);
    } else if (CFStringHasSuffix(fpath, CFSTR(".pbp")) ||
               CFStringHasSuffix(fpath, CFSTR(".PBP"))) {
        image = PBPPreview(fpath);
    }

    if (image == NULL || QLPreviewRequestIsCancelled(preview)) {
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

    ctx = QLPreviewRequestCreateContext(preview, size, false, NULL);

    if (ctx) {
        CGContextDrawImage(ctx, rect, new_image);
        QLPreviewRequestFlushContext(preview, ctx);
        CFRelease(ctx);
    }

    CGImageRelease(new_image);

    return noErr;
}

void CancelPreviewGeneration(void *thisInterface,
                             QLPreviewRequestRef preview) {
    // Implement only if supported
}
