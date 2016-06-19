#include "CImg.h"

using namespace cimg_library;

int main()
{
  CImg<unsigned char> image(512,512);
  const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 };
  cimg_forXY(image,x,y)
    image(x,y)=123;
  int stepx=3;
  int stepy=3;
  for (int y=0; y<(int)(image._height); y+=stepy)
    for (int x=0; x<(int)(image._width); x+=stepx)
      image(x,y)=0;
  image.display("node grid");
  return 0;
}
