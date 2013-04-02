#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#include <Cocoa/Cocoa.h>

#include "cdio/iso9660.h"

OSStatus GenerateThumbnailForURL(void *thisInterface, QLThumbnailRequestRef thumbnail, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options, CGSize maxSize);
void CancelThumbnailGeneration(void *thisInterface, QLThumbnailRequestRef thumbnail);


// https://github.com/lnxbil/quicklook-pfm/blob/master/src/GenerateThumbnailForURL.m
CGContextRef CreateARGBBitmapContext(CGSize size)
{
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
    if (colorSpace == NULL)
    {
        fprintf(stderr, "Error allocating color space\n");
        return NULL;
    }

    // Allocate memory for image data. This is the destination in memory
    // where any drawing to the bitmap context will be rendered.
    bitmapData = malloc(bitmapByteCount);
    if (bitmapData == NULL)
    {
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
                                    8,      // bits per component
                                    bitmapBytesPerRow,
                                    colorSpace,
                                    kCGImageAlphaPremultipliedFirst);
    if (context == NULL)
    {
        free(bitmapData);
        fprintf(stderr, "Context not created!");
    }

    // Make sure and release colorspace before returning
    CGColorSpaceRelease(colorSpace);

    return context;
}


char * MYCFStringCopyUTF8String(CFStringRef aString) {
    if (aString == NULL) {
        return NULL;
    }

    CFIndex length = CFStringGetLength(aString);
    CFIndex maxSize =
    CFStringGetMaximumSizeForEncoding(length,
                                      kCFStringEncodingUTF8);
    char *buffer = (char *)malloc(maxSize);
    if (CFStringGetCString(aString, buffer, maxSize,
                           kCFStringEncodingUTF8)) {
        return buffer;
    }
    return NULL;
}

// http://www.gnu.org/software/libcdio/libcdio.html#Example-4
CGImageRef pspIsoThumbnail(CFStringRef fpath) {
    char* fpath_cstr = MYCFStringCopyUTF8String(fpath);
    iso9660_stat_t *p_statbuf;

    iso9660_t *p_iso = iso9660_open(fpath_cstr);
    free(fpath_cstr);
    if (p_iso == NULL) {
        // TODO ERROR
    }
    
    p_statbuf = iso9660_ifs_stat_translate(p_iso, "PSP_GAME/ICON0.PNG");
    
    if (p_statbuf == NULL) {
        // TODO ERROR, return
        iso9660_close(p_iso);
    } else {
        NSLog(@"GOT %p, %d", p_statbuf, p_statbuf->size);
    }
    
    // copy content
    int i;
    for (i = 0; i < p_statbuf->size; i += ISO_BLOCKSIZE) {
        char buf[ISO_BLOCKSIZE];
        memset(buf, 0, ISO_BLOCKSIZE);
        if (iso9660_iso_seek_read(p_iso, buf, p_statbuf->lsn + (i / ISO_BLOCKSIZE), 1) != ISO_BLOCKSIZE) {
            // TODO ERROR
        }
        
    }
    
    // need to truncate fetched data to p_statbuf->size
    
    return NULL;
}

CGImageRef pbpThumbnail(CFStringRef fpath) {
    char* fpath_cstr = MYCFStringCopyUTF8String(fpath);
    FILE* fp = fopen(fpath_cstr, "rb");
    free(fpath_cstr);

    char magic[4];
    fread(magic, 1, 4, fp);
    printf("magic: 0x%02x 0x%02x 0x%02x 0x%02x ('\\%d%c%c%c')\n",
           magic[0], magic[1], magic[2], magic[3],
           magic[0], magic[1], magic[2], magic[3]);

    int32_t offst;
    fseek(fp, 8, SEEK_SET);
    fread(&offst, sizeof(int32_t), 1, fp);
    printf("offset of param.sfo: 0x%04x\n", offst);

    fseek(fp, 12, SEEK_SET);
    fread(&offst, sizeof(int32_t), 1, fp);
    printf("offset of icon0.png: 0x%04x\n", offst);

    fseek(fp, 16, SEEK_SET);
    fread(&offst, sizeof(int32_t), 1, fp);
    printf("offset of icon1.pmf: 0x%04x\n", offst);

    fseek(fp, 24, SEEK_SET);
    fread(&offst, sizeof(int32_t), 1, fp);
    printf("offset of pic1.png: 0x%04x\n", offst);

    fseek(fp, 28, SEEK_SET);
    fread(&offst, sizeof(int32_t), 1, fp);
    printf("offset of snd0.at3: 0x%04x\n", offst);

    fseek(fp, 32, SEEK_SET);
    fread(&offst, sizeof(int32_t), 1, fp);
    printf("offset of PSP data: 0x%04x\n", offst);

    fseek(fp, 36, SEEK_SET);
    fread(&offst, sizeof(int32_t), 1, fp);
    printf("offset of PSAR data: 0x%04x\n", offst);

    // TODO: get the image
    return NULL;
}


// http://dsibrew.org/wiki/NDS_Format
// https://code.google.com/p/tinke/source/browse/trunk/Tinke/Nitro/NDS.cs
// http://crackerscrap.com/docs/dsromstructure.html
CGImageRef ndsThumbnail(CFStringRef fpath) {
    NSBitmapImageRep * bmp = [[NSBitmapImageRep alloc]
                              initWithBitmapDataPlanes:NULL
                              pixelsWide:32 pixelsHigh:32 bitsPerSample:8
                              samplesPerPixel:3 hasAlpha:NO isPlanar:NO
                              colorSpaceName:NSDeviceRGBColorSpace
                              bytesPerRow:(32 * 3) bitsPerPixel:24];

    if (bmp == NULL) {
        return NULL;
    }

    char* fpath_cstr = MYCFStringCopyUTF8String(fpath);
    FILE* fp = fopen(fpath_cstr, "rb");
    free(fpath_cstr);

    char game_title[13];
    memset(game_title, 0, sizeof(game_title));

    fread(game_title, 1, 12, fp);
    printf("game title: %s\n", game_title);

    char game_code[5];
    memset(game_code, 0, sizeof(game_code));

    fseek(fp, 12, SEEK_SET);
    fread(game_code, 1, 4, fp);
    printf("game id: %s\n", game_code);

    char maker_code[3];
    memset(maker_code, 0, sizeof(maker_code));

    fseek(fp, 16, SEEK_SET);
    fread(maker_code, 1, 2, fp);
    printf("maker code: %s\n", maker_code);

    unsigned char romver;
    fseek(fp, 0x1e, SEEK_SET);
    fread(&romver, 1, 1, fp);
    printf("rom ver: %d\n", (int) romver);

    int32_t icon_title_offst;
    fseek(fp, 0x68, SEEK_SET);
    fread(&icon_title_offst, sizeof(int32_t), 1, fp);
    assert(ftell(fp) == 0x6c);
    printf("icon/title offst: 0x%04x\n", icon_title_offst);

    int16_t ver;
    fseek(fp, icon_title_offst + 0x0, SEEK_SET);
    fread(&ver, sizeof(int16_t), 1, fp);
    printf("version: %d\n", ver);
//
//    char long_title[256 * 6 + 1];
//    char long_title_raw[256 + 1];
//    memset(long_title_raw, 0, sizeof(long_title_raw));
//    fseek(fp, icon_title_offst + 0x240, SEEK_SET);
//    fread(long_title_raw, 1, 156, fp);
//
////    code_convert("utf-16le", "utf-8", long_title_raw, sizeof(long_title_raw), long_title, sizeof(long_title));
////    printf("japanese title: %s\n", long_title);
//
//    memset(long_title_raw, 0, sizeof(long_title_raw));
//    fseek(fp, icon_title_offst + 0x340, SEEK_SET);
//    fread(long_title_raw, 1, 156, fp);
////    code_convert("utf-16le", "utf-8", long_title_raw, sizeof(long_title_raw), long_title, sizeof(long_title));
////    printf("english title: %s\n", long_title);
//
//    memset(long_title_raw, 0, sizeof(long_title_raw));
//    fseek(fp, icon_title_offst + 0x440, SEEK_SET);
//    fread(long_title_raw, 1, 156, fp);
////    code_convert("utf-16le", "utf-8", long_title_raw, sizeof(long_title_raw), long_title, sizeof(long_title));
////    printf("french title: %s\n", long_title);
//
//    memset(long_title_raw, 0, sizeof(long_title_raw));
//    fseek(fp, icon_title_offst + 0x540, SEEK_SET);
//    fread(long_title_raw, 1, 156, fp);
////    code_convert("utf-16le", "utf-8", long_title_raw, sizeof(long_title_raw), long_title, sizeof(long_title));
////    printf("german title: %s\n", long_title);
//
//    memset(long_title_raw, 0, sizeof(long_title_raw));
//    fseek(fp, icon_title_offst + 0x640, SEEK_SET);
//    fread(long_title_raw, 1, 156, fp);
////    code_convert("utf-16le", "utf-8", long_title_raw, sizeof(long_title_raw), long_title, sizeof(long_title));
////    printf("italian title: %s\n", long_title);
//
//    memset(long_title_raw, 0, sizeof(long_title_raw));
//    fseek(fp, icon_title_offst + 0x740, SEEK_SET);
//    fread(long_title_raw, 1, 156, fp);
////    code_convert("utf-16le", "utf-8", long_title_raw, sizeof(long_title_raw), long_title, sizeof(long_title));
////    printf("spanish title: %s\n", long_title);

    fseek(fp, icon_title_offst + 0x20, SEEK_SET);
    unsigned char icon[512];


    uint16_t palette[16];
    fread(icon, 1, 512, fp);
    fread(palette, sizeof(uint16_t), 16, fp);

    uint8 * data = (uint8 *) bmp.bitmapData;
//    for (int y = 0; y < 32; y++) {
//        for (int x = 0; x < 16; x++) {
//            data[y * 32 * 3 + 3 * x] = 128; // r
//            data[y * 32 * 3 + 3 * x + 1] = 128; // g
//            data[y * 32 * 3 + 3 * x + 2] = 128; // b
//        }
//    }

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



/* -----------------------------------------------------------------------------
    Generate a thumbnail for file

   This function's job is to create thumbnail for designated file as fast as possible
   ----------------------------------------------------------------------------- */

OSStatus GenerateThumbnailForURL(void *thisInterface, QLThumbnailRequestRef thumbnail, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options, CGSize maxSize)
{
    CFStringRef fpath = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);


    if (QLThumbnailRequestIsCancelled(thumbnail)) {
        return noErr;
    }

    CGImageRef image = NULL;

    if (CFStringHasSuffix(fpath, CFSTR(".nds")) || CFStringHasSuffix(fpath, CFSTR(".NDS"))) {
        NSLog(@"HI nds");
        image = ndsThumbnail(fpath);
    } else if (CFStringHasSuffix(fpath, CFSTR(".pbp")) || CFStringHasSuffix(fpath, CFSTR(".PBP"))) {
        image = pbpThumbnail(fpath);
    } else if (CFStringHasSuffix(fpath, CFSTR(".iso")) || CFStringHasSuffix(fpath, CFSTR(".ISO"))) {
        image = pspIsoThumbnail(fpath);
    }

    if (image == NULL) {
        return noErr;
    }

    size_t w = 100;
    size_t h = 100;


    @autoreleasepool {
        CGSize size = CGSizeMake(w, h);
        CGRect rect = {{0, 0}, {size.width, size.height}};

        CGContextRef ctx = CreateARGBBitmapContext(size);
        CGContextDrawImage(ctx, rect, image);
        CGImageRef new_image = CGBitmapContextCreateImage(ctx);
        CGContextRelease(ctx);

        QLThumbnailRequestSetImage(thumbnail, new_image, NULL);
        CGImageRelease(new_image);
    }

    return noErr;
}

void CancelThumbnailGeneration(void *thisInterface, QLThumbnailRequestRef thumbnail)
{
    // Implement only if supported
}
