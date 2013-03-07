#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <string>
#include <algorithm>

#include <iconv.h>

extern "C" {
#include "im.h"
#include "imbmp.h"
}

using namespace std;

bool endswith(const string& str, const string& tail) {
    if (str.size() < tail.size()) {
        return false;
    } else {
        return str.substr(str.size() - tail.size()) == tail;
    }
}

int code_convert(const char *from_charset, const char *to_charset, char *inbuf, size_t inlen, char *outbuf, size_t outlen) {
    iconv_t cd;
    char **pin = &inbuf;
    char **pout = &outbuf;
    cd = iconv_open(to_charset, from_charset);
    if (cd == 0)
        return -1;
    memset(outbuf, 0, outlen);
    if (iconv(cd, pin, &inlen, pout, &outlen) == (size_t) -1)
        return -1;
    iconv_close(cd);
    return 0;
}

// http://dsibrew.org/wiki/NDS_Format
// https://code.google.com/p/tinke/source/browse/trunk/Tinke/Nitro/NDS.cs
// http://crackerscrap.com/docs/dsromstructure.html
void ndsinfo(FILE* fp) {
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
    memset(maker_code, 0, sizeof(game_code));

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

    char long_title[256 * 6 + 1];
    char long_title_raw[256 + 1];
    memset(long_title_raw, 0, sizeof(long_title_raw));
    fseek(fp, icon_title_offst + 0x240, SEEK_SET);
    fread(long_title_raw, 1, 156, fp);
    code_convert("utf-16le", "utf-8", long_title_raw, sizeof(long_title_raw), long_title, sizeof(long_title));
    printf("japanese title: %s\n", long_title);

    memset(long_title_raw, 0, sizeof(long_title_raw));
    fseek(fp, icon_title_offst + 0x340, SEEK_SET);
    fread(long_title_raw, 1, 156, fp);
    code_convert("utf-16le", "utf-8", long_title_raw, sizeof(long_title_raw), long_title, sizeof(long_title));
    printf("english title: %s\n", long_title);

    memset(long_title_raw, 0, sizeof(long_title_raw));
    fseek(fp, icon_title_offst + 0x440, SEEK_SET);
    fread(long_title_raw, 1, 156, fp);
    code_convert("utf-16le", "utf-8", long_title_raw, sizeof(long_title_raw), long_title, sizeof(long_title));
    printf("french title: %s\n", long_title);

    memset(long_title_raw, 0, sizeof(long_title_raw));
    fseek(fp, icon_title_offst + 0x540, SEEK_SET);
    fread(long_title_raw, 1, 156, fp);
    code_convert("utf-16le", "utf-8", long_title_raw, sizeof(long_title_raw), long_title, sizeof(long_title));
    printf("german title: %s\n", long_title);

    memset(long_title_raw, 0, sizeof(long_title_raw));
    fseek(fp, icon_title_offst + 0x640, SEEK_SET);
    fread(long_title_raw, 1, 156, fp);
    code_convert("utf-16le", "utf-8", long_title_raw, sizeof(long_title_raw), long_title, sizeof(long_title));
    printf("italian title: %s\n", long_title);

    memset(long_title_raw, 0, sizeof(long_title_raw));
    fseek(fp, icon_title_offst + 0x740, SEEK_SET);
    fread(long_title_raw, 1, 156, fp);
    code_convert("utf-16le", "utf-8", long_title_raw, sizeof(long_title_raw), long_title, sizeof(long_title));
    printf("spanish title: %s\n", long_title);


    // export the icon
    fseek(fp, icon_title_offst + 0x20, SEEK_SET);
    unsigned char icon[512];


    uint16_t palette[16];
    fread(icon, 1, 512, fp);
    fread(palette, sizeof(uint16_t), 16, fp);

    image im = im_new(32, 32);
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
            pixel pxl = im_pixel(im, x, y);
            if (px_l != 0) {
                px_r(pxl) = (palette[px_l] & 0x1F) * 8;
                px_g(pxl) = ((palette[px_l] >> 5) & 0x1F) * 8;
                px_b(pxl) = ((palette[px_l] >> 10) & 0x1F) * 8;
            }

            pxl = im_pixel(im, x + 1, y);
            if (px_r != 0) {
                px_r(pxl) = (palette[px_r] & 0x1F) * 8;
                px_g(pxl) = ((palette[px_r] >> 5) & 0x1F) * 8;
                px_b(pxl) = ((palette[px_r] >> 10) & 0x1F) * 8;

            }
        }
    }


    imbmp_save(im, "nds.bmp");
    im_free(im);
}

// http://mc.pp.se/psp/pbp.xhtml
void pbpinfo(FILE* fp) {
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
}

void rominfo(const string& fpath) {
    FILE* fp = fopen(fpath.c_str(), "rb");
    if (fp == NULL) {
        printf("cannot open: %s\n", fpath.c_str());
    } else {
        printf("rom: %s\n", fpath.c_str());
        string lower_fpath = fpath;
        std::transform(fpath.begin(), fpath.end(), lower_fpath.begin(), ::tolower);
        if (endswith(lower_fpath, ".pbp")) {
            pbpinfo(fp);
        } else if (endswith(lower_fpath, ".nds")) {
            ndsinfo(fp);
        }
        fclose(fp);
    }
    printf("----\n");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("usage: %s <romfiles..>\n", argv[0]);
        printf("supported roms: .pbp, .nds\n");
        exit(0);
    }
    for (int i = 1; i < argc; i++) {
        rominfo(argv[i]);
    }
    return 0;
}
