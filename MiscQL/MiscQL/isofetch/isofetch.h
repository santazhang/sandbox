//
//  isofetch.h
//
//  Created by Santa Zhang on 2/27/15.
//  Copyright (c) 2015 Santa Zhang. All rights reserved.
//

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // #ifdef __cplusplus

void* iso_open(const char* iso_fpath);
const char* iso_fetch(void* iso, const char* entry_path,
                      int64_t* size, const char** error);
void iso_close(void* iso);

#ifdef __cplusplus
}  // extern "C"
#endif  // #ifdef __cplusplus
