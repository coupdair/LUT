#include "CImg.h"

using namespace cimg_library;

int main()
{
  CImg<unsigned char> image(512,512);
  const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 };
  cimg_forXY(image,x,y)
    image(x,y)=123;
  image.display("123");
  return 0;
}
