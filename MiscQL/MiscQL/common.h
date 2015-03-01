//
//  common.h
//  MiscQL
//
//  Created by Santa Zhang on 6/16/13.
//  Copyright (c) 2013 Santa Zhang. All rights reserved.
//

#ifndef MiscQL_common_h
#define MiscQL_common_h


#include <Cocoa/Cocoa.h>


// isofetch.c
char* isofetch(char* iso_fpath, char* entry_name, int* datalen);


// common.m
char* MYCFStringCopyUTF8String(CFStringRef str);
CGContextRef CreateARGBBitmapContext(CGSize size);


#endif
