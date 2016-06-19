#include "CImg.h"

using namespace cimg_library;

int main()
{
  CImg<unsigned char> image(253,253,1,3);
  const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 }, black[] = { 0,0,0 };
  image.fill(0);
/*
  int stepx=4;
  int stepy=4;
  //draw horizontal lines
  for(int y=0; y<(int)(image.height()); y+=stepy)
    image.draw_line(0,y,image.width()-1,y,blue);
  //draw vertical lines
  for(int x=0; x<(int)(image.width()); x+=stepx)
    image.draw_line(x,0,x,image.height()-1,red);
*/
  image.display("zoom in");
  return 0;
}
