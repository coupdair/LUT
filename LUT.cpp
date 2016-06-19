#include "CImg.h"

using namespace cimg_library;

int main()
{
  int width=253;
  int height=128;
  int zoom=4;int step=zoom;
  CImg<unsigned char> image(width,height,1,3);
  const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 }, black[] = { 0,0,0 }, white[] = { 222,222,222 };
  //partition color
  image.fill(255);//Pall
  image.draw_rectangle(0,0,127,height,red);//P128
  image.draw_rectangle(128,0,128+64,height,blue);//P64
  image.draw_rectangle(128+64+1,0,128+64+1+32,height,white);//P64
  //partition sum
  image.draw_line(0,0,image.width()-1,0,black);
  image.draw_line(0,0,0,image.height()-1,black);
  //set few points
  int i=123;int j=123;
  image.draw_point(i,j,green);//IP
  image.draw_point(i,0,green);//sum
  image.draw_point(0,j,green);//sum
  i=12;j=21;
  image.draw_point(i,j,green);//IP
  image.draw_point(i,0,green);//sum
  image.draw_point(0,j,green);//sum
  //zoom
  image.resize(-zoom*100,-zoom*100);
  //draw separations
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
