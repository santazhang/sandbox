#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#include <Cocoa/Cocoa.h>

extern CGContextRef CreateARGBBitmapContext(CGSize size);

extern char * MYCFStringCopyUTF8String(CFStringRef str);

// isofetch.c
extern char* isofetch(char* iso_fpath, char* entry_name, int* datalen);

OSStatus GeneratePreviewForURL(void *thisInterface, QLPreviewRequestRef preview, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options);
void CancelPreviewGeneration(void *thisInterface, QLPreviewRequestRef preview);

CGImageRef PSPISOPreview(CFStringRef fpath) {
    char * fpath_cstr = MYCFStringCopyUTF8String(fpath);
    int datalen = -1;
    char* pngdata = isofetch(fpath_cstr, "PSP_GAME/PIC1.PNG", &datalen);
    free(fpath_cstr);
    
    if (pngdata == NULL) {
        return NULL;
    }
    
    //    NSLog(@"%d %d %d %d %d", pngdata[0], pngdata[1], pngdata[2], pngdata[3], datalen);
    
    NSData * nsdata = [NSData dataWithBytesNoCopy:pngdata length:datalen freeWhenDone:YES];
    CGDataProviderRef imgdata_provider = CGDataProviderCreateWithCFData((__bridge CFDataRef) nsdata);
    CGImageRef image = CGImageCreateWithPNGDataProvider(imgdata_provider, NULL, true, kCGRenderingIntentDefault);
    
    //    NSLog(@"GOT %p %p %p", nsdata, imgdata_provider, image);
    
    
    // dont free
    //    free(pngdata);
    return image;
}

/* -----------------------------------------------------------------------------
   Generate a preview for file

   This function's job is to create preview for designated file
   ----------------------------------------------------------------------------- */

OSStatus GeneratePreviewForURL(void *thisInterface, QLPreviewRequestRef preview, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options)
{
    CFStringRef fpath = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);

    if (QLPreviewRequestIsCancelled(preview)) {
        return noErr;
    }
    
    CGImageRef image = NULL;
    
    if (CFStringHasSuffix(fpath, CFSTR(".iso")) || CFStringHasSuffix(fpath, CFSTR(".ISO"))) {
        image = PSPISOPreview(fpath);
    }
    
    if (image == NULL || QLPreviewRequestIsCancelled(preview)) {
        return noErr;
    }
    
    size_t w = CGImageGetWidth(image);
    size_t h = CGImageGetHeight(image);
    
    @autoreleasepool {
        CGSize size = CGSizeMake(w, h);
        CGRect rect = {{0, 0}, {size.width, size.height}};
        
        CGContextRef ctx = CreateARGBBitmapContext(size);
        CGContextDrawImage(ctx, rect, image);
        CGImageRef new_image = CGBitmapContextCreateImage(ctx);
        CGContextRelease(ctx);
        
        ctx = QLPreviewRequestCreateContext(preview, *(CGSize *)&size, false, NULL);
        
        if(ctx) {
            CGContextDrawImage(ctx, rect, new_image);
            QLPreviewRequestFlushContext(preview, ctx);
            CFRelease(ctx);
        }
        
        // new_image
        CGImageRelease(new_image);
    }
    
    
    return noErr;
}

void CancelPreviewGeneration(void *thisInterface, QLPreviewRequestRef preview)
{
    // Implement only if supported
}
