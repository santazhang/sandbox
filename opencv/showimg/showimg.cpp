#include "cv.h"
#include "highgui.h"

#include <cstdio>

using namespace std;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Usage: showimg image-path\n");
    return 0;
  }

  IplImage* pImg = cvLoadImage(argv[1]);
  cvNamedWindow("hello");
  cvShowImage("hello", pImg);
  cvWaitKey(0);
  cvDestroyWindow("hello");
  cvReleaseImage(&pImg);
  return 0;
}

