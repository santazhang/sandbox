//
//  isofetch.cpp
//
//  Created by Santa Zhang on 2/27/15.
//  Copyright (c) 2015 Santa Zhang. All rights reserved.
//

#include "archive.h"
#include "isofetch.h"

#include <string>
#include <map>

#include <stdio.h>

namespace {

class ISO {
    // Factory pattern
    ISO(FILE* fp);

    // Disable copy
    ISO(const ISO&);
    const ISO& operator= (const ISO&);

public:
    static ISO* open(const char* fpath) {
        FILE* fp = fopen(fpath, "rb");
        if (fp == nullptr) {
            return nullptr;
        }
        return new ISO(fp);
    }

    void close();

private:
    FILE* fp_;
    struct archive *archive_;
};


ISO::ISO(FILE* fp): fp_(fp) {
    archive_ = archive_read_new();
    archive_read_support_format_iso9660(archive_);
}

void ISO::close() {
    archive_read_close(archive_);
    archive_read_free(archive_);
    fclose(fp_);
}

}  // default namespace


#ifdef __cplusplus
extern "C" {
#endif  // #ifdef __cplusplus

void* iso_open(const char* iso_fpath) {
    return reinterpret_cast<void*>(ISO::open(iso_fpath));
}

const char* iso_fetch(void* iso, const char* entry_path,
                      int64_t* size, const char** error) {
    *error = nullptr;
    *size = -1;

    if (iso == nullptr) {
        *error = "Invalid ISO handler";
        return nullptr;
    }

    // TODO
    return nullptr;
}

void iso_close(void* iso) {
    if (iso != nullptr) {
        reinterpret_cast<ISO*>(iso)->close();
    }
}

#ifdef __cplusplus
}  // extern "C"
#endif  // #ifdef __cplusplus
