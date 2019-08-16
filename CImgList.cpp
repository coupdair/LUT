#include "../CImg/CImg.h"
using namespace cimg_library;
int main() {
  CImg<unsigned char> image1(12,23,1,1,123);
  CImg<unsigned char> image2(23,34,1,1,234);
  image1.print("image1",false);
  image2.print("image2",false);

  CImgList<unsigned char> images(image1,image2,true);
  
  images.print("images",false);

  printf("images(0,1)=%d\n",images(0,1));
  printf("images(1,2)=%d\n",images(1,2));

  return 0;
}
