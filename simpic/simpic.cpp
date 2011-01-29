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

IplImage* gray_scale_image(IplImage* image) {
  IplImage* new_image = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
  cvCvtColor(image, new_image, CV_RGB2GRAY);
  return new_image;
}

void normalize_gray_image(IplImage* gray) {
  int area = gray->width * gray->height;
  int i, sum;
  double scale = 255. / area;
  const int hist_sz = 256;
  int hist[hist_sz];
  int lut[hist_sz + 1];
  for (i = 0; i < hist_sz; i++) {
    hist[i] = 0;
    lut[i] = 0;
  }
  CvScalar s;
  for (int row = 0; row < gray->height; row++) {
    for (int col = 0; col < gray->width; col++) {
      s = cvGet2D(gray, row, col);
      hist[(int) s.val[0]]++;
    }
  }
  sum = 0;
  for (i = 0; i < hist_sz; i++) {
    sum += hist[i];
    int val = (int)(sum * scale);
    if (val > 255) {
      val = 255;
    }
    lut[i] = val;
  }
  lut[0] = 0;
  for (int row = 0; row < gray->height; row++) {
    for (int col = 0; col < gray->width; col++) {
      s = cvGet2D(gray, row, col);
      s.val[0] = lut[(int) s.val[0]];
      cvSet2D(gray, row, col, s);
    }
  }
}

PicFP::PicFP(const string& pic_fn) : fn(pic_fn) {
  for (int i = 0; i < 27; i++) {
    data[i] = 0;
  }
  IplImage* pic = cvLoadImage(fn.c_str());
  if (pic == NULL) {
    printf("*** bad image: '%s'\n", fn.c_str());
    fn = "";
  } else if (pic->height <= 20 || pic->width <= 20) {
    printf("*** skip image: '%s', %d x %d\n", fn.c_str(), pic->width, pic->height);
    fn = "";
  } else {
    IplImage* gray = gray_scale_image(pic);
    normalize_gray_image(gray);
    int h[12];
    CvScalar s;
    for (int i = 0; i < 12; i++) {
      h[i] = 0;
    }
    int half_height = gray->height / 2;
    int half_width = gray->width / 2;
    for (int row = 0; row < gray->height; row++) {
      for (int col = 0; col < gray->width; col++) {
        s = cvGet2D(gray, row, col);
        int* hx;
        if (row < half_height && col < half_width) {
          hx = h;
        } else if (row < half_height && col >= half_width) {
          hx = h + 3;
        } else if (row >= half_height && col < half_width) {
          hx = h + 6;
        } else if (row >= half_height && col >= half_width) {
          hx = h + 9;
        }
        if (s.val[0] < 64) {
          hx[0]++;
        } else if (s.val[0] < 128) {
          hx[1]++;
        } else if (s.val[0] < 192) {
          hx[2]++;
        }
      }
    }
    
    // size of 4 regions
    int s0 = half_width * half_height;
    int s1 = (gray->width - half_width) * half_height;
    int s2 = half_width * (gray->height - half_height);
    int s3 = (gray->width - half_width) * (gray->height - half_height);
    
    data[0] = (byte) (255 * h[0] / s0);
    data[1] = (byte) (255 * h[1] / s0);
    data[2] = (byte) (255 * h[2] / s0);
    
    data[3] = (byte) (255 * h[3] / s1);
    data[4] = (byte) (255 * h[4] / s1);
    data[5] = (byte) (255 * h[5] / s1);
    
    data[6] = (byte) (255 * h[6] / s2);
    data[7] = (byte) (255 * h[7] / s2);
    data[8] = (byte) (255 * h[8] / s2);
    
    data[9] = (byte) (255 * h[9] / s3);
    data[10] = (byte) (255 * h[10] / s3);
    data[11] = (byte) (255 * h[11] / s3);
    
    data[12] = (byte) (255 * (h[0] + h[3]) / (s0 + s1));
    data[13] = (byte) (255 * (h[1] + h[4]) / (s0 + s1));
    data[14] = (byte) (255 * (h[2] + h[5]) / (s0 + s1));
    
    data[15] = (byte) (255 * (h[6] + h[9]) / (s2 + s3));
    data[16] = (byte) (255 * (h[7] + h[10]) / (s2 + s3));
    data[17] = (byte) (255 * (h[8] + h[11]) / (s2 + s3));
    
    data[18] = (byte) (255 * (h[0] + h[6]) / (s0 + s2));
    data[19] = (byte) (255 * (h[1] + h[7]) / (s0 + s2));
    data[20] = (byte) (255 * (h[2] + h[8]) / (s0 + s2));
    
    data[21] = (byte) (255 * (h[3] + h[9]) / (s1 + s3));
    data[22] = (byte) (255 * (h[4] + h[10]) / (s1 + s3));
    data[23] = (byte) (255 * (h[5] + h[11]) / (s1 + s3));
    
    data[24] = (byte) (255 * (h[0] + h[3] + h[6] + h[9]) / (s0 + s1 + s2 + s3));
    data[25] = (byte) (255 * (h[1] + h[4] + h[7] + h[10]) / (s0 + s1 + s2 + s3));
    data[26] = (byte) (255 * (h[2] + h[5] + h[8] + h[11]) / (s0 + s1 + s2 + s3));
    
    printf("fingerprint:");
    for (int i = 0; i < 27; i++) {
      printf(" %d", data[i]);
    }
    printf("\n");
    
    cvReleaseImage(&gray);
  }
  cvReleaseImage(&pic);
}

vector<PicFP> all_pic;

static const char* image_exts[] = {".jpg", ".png", ".bmp", NULL};

static const int region_width[] = {1, 1, 1, 1, 2, 2, 1, 1, 2};
static const int region_height[] = {1, 1, 1, 1, 1, 1, 2, 2, 2};

// if not valid, -1 will be returned
double fingerprint_distance(const PicFP& fp1, const PicFP& fp2) {
  if (fp1.fn == "" || fp2.fn == "") {
    return -1;
  }
  double d = 0;
  for (int i = 0; i < 4; i++) {
    double region_d = 0;
    double ha0 = fp1.data[i * 3] / 255.0;
    double ha1 = fp1.data[i * 3 + 1] / 255.0;
    double ha2 = fp1.data[i * 3 + 2] / 255.0;
    double ha3 = 1.0 - ha0 - ha1 - ha2;
    double hb0 = fp2.data[i * 3] / 255.0;
    double hb1 = fp2.data[i * 3 + 1] / 255.0;
    double hb2 = fp2.data[i * 3 + 2] / 255.0;
    double hb3 = 1.0 - hb0 - hb1 - hb2;
    region_d = (ha0 - hb0) * (ha0 - hb0) + (ha1 - hb1) * (ha1 - hb1) + (ha2 - hb2) * (ha2 - hb2) + (ha3 - hb3) * (ha3 - hb3);
    d += region_d;
  }
  return d;
}

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
      struct stat st;
      if (stat(fpath.c_str(), &st) == 0) {
        if (S_ISREG(st.st_mode) && is_image(fpath.c_str())) {
          printf("fingerprinting: %s\n", fpath.c_str());
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
  FILE* fp = fopen("simpic.csv", "w");
  fprintf(fp, "pic1,pic2,distance\n");
  for (int i = 0; i < all_pic.size(); i++){
    for (int j = i + 1; j < all_pic.size(); j++) {
      double d = fingerprint_distance(all_pic[i], all_pic[j]);
      if (d >= 0 && d < 1.0) {
        printf("distance: %s, %s, %f\n", all_pic[i].fn.c_str(), all_pic[j].fn.c_str(), d);
        fprintf(fp, "%s,%s,%f\n", all_pic[i].fn.c_str(), all_pic[j].fn.c_str(), d);
      }
    }
  }
  fclose(fp);
  return 0;
}
