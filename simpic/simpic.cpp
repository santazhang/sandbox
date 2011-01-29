#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <vector>
#include <string>
#include <map>

#include "cv.h"
#include "highgui.h"

using namespace std;

typedef unsigned char byte;

// finger print of picture
class PicFP {
public:
  PicFP() : fn("") {}
  PicFP(const string& pic_fn);
  
  // picture filename
  string fn;
  
  // histogram data of the picture
  // 9 groups
  // for each group data[X], data[X + 1], data[X + 2] is histogram percentage of gray scale 0~63, 64~127, 128~191
  // value for 192~255 could be caculated as 4 * 256 - data[X] - data[X + 1] - data[X + 2]
  // the actual percentage info could be caculated by data[Y] / 255.0
  byte data[27];  // 3 * 9
};

PicFP::PicFP(const string& pic_fn) : fn(pic_fn) {
  IplImage* pic = cvLoadImage(fn.c_str());
  cvReleaseImage(&pic);
}

vector<PicFP> all_pic;

static const char* image_exts[] = {".jpg", ".png", ".gif", ".bmp", NULL};

static int strcmp_ignore_case(const char* str1, const char* str2) {
  int i;
  for (i = 0; str1[i] != '\0' && str2[i] != '\0'; i++) {
    char ch1 = str1[i];
    if ('A' <= ch1 && ch1 <= 'Z') {
      // to lowercase
      ch1 = (char) (ch1 - 'A' + 'a');
    }
    char ch2 = str2[i];
    if ('A' <= ch2 && ch2 <= 'Z') {
      // to lowercase
      ch2 = (char) (ch2 - 'A' + 'a');
    }
    if (ch1 < ch2) {
      return -1;
    } else if (ch1 > ch2) {
      return 1;
    }
  }
  if (str1[i] != '\0') {
    return 1;
  } else if (str2[i] != '\0') {
    return -1;
  } else {
    return 0;
  }
}

bool is_image(const char* fn) {
  for (int i = 0; image_exts[i] != NULL; i++) {
    const char* ext = image_exts[i];
    if (strlen(ext) > strlen(fn)) {
      continue;
    }
    if (strcmp_ignore_case(fn + strlen(fn) - strlen(ext), ext) == 0) {
      return true;
    }
  }
  return false;
}

void walk_image_root(const char* image_root) {
  string fpath;
  DIR* p_dir = opendir(image_root);
  if (p_dir != NULL) {
    dirent* p_ent;
    while ((p_ent = readdir(p_dir)) != NULL) {
      if (p_ent->d_name[0] == '.' || p_ent->d_name[0] == '~' || p_ent->d_name[0] == '#') {
        // skip hidden files
        continue;
      }
      fpath = string(image_root) + "/" + string(p_ent->d_name);
      printf("fingerprinting: %s\n", fpath.c_str());
      struct stat st;
      if (stat(fpath.c_str(), &st) == 0) {
        if (S_ISREG(st.st_mode) && is_image(fpath.c_str())) {
          all_pic.push_back(PicFP(fpath.c_str()));
        } else if (S_ISDIR(st.st_mode)) {
          walk_image_root(fpath.c_str());
        }
      }
    }
    closedir(p_dir);
  }
}

int main(int argc, char* argv[]) {
  if (argc == 1) {
    printf("usage: simpic <image_root>\n");
    exit(0);
  }
  // walk all pictures
  const char* image_root = argv[1];
  walk_image_root(image_root);
  
  return 0;
}
