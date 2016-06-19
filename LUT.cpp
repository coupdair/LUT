#include "CImg.h"

using namespace cimg_library;

int main()
{
  int width=253;
  int height=128;
  int zoom=4;int step=zoom;
  CImg<unsigned char> image(width*step,height*step,1,3);
  const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 }, black[] = { 0,0,0 }, white[] = { 222,222,222 };
  image.fill(white[0]);//white
  int stepx=step;
  int stepy=step;
  //draw horizontal lines
  for(int y=0; y<(int)(image.height()); y+=stepy)
    image.draw_line(0,y,image.width()-1,y,black);
  //draw vertical lines
  for(int x=0; x<(int)(image.width()); x+=stepx)
    image.draw_line(x,0,x,image.height()-1,black);
  image.display("zoom 4x");
  image.save("253x128_4x.png");
  return 0;
}
