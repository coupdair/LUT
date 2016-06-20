#include "CImg.h"

using namespace cimg_library;

#define R 0
#define G 1
#define B 2
#define V  5 //Value, i.e. IP
#define P  3 //Partition
#define SP 4 //SubPartition

int main()
{
  int width=257;
  int height=128;
  const int spectrum=6;
  int zoom=7;int step=zoom;
  CImg<unsigned char> image(width,height,1,spectrum);
  //                              R   G   B    P  SP   V
  const unsigned char red[] = { 255,  0,  0,   0,  0,  0 }
                  , green[] = { 0  ,255,  0,   0,  0,  0 }
                  ,  blue[] = { 0  ,  0,255,   0,  0,  0 }
                  , black[] = { 0  ,  0,  0,   0,  0,  0 }
                  , white[] = { 222,222,222,   0,  0,  0 }
                  , cP128[] = {  64, 64, 64, 128,  0,255 }
                  , cP64[]  = { 128,128,128,  64,  0,255 }
                  , cP32[]  = { 196,196,196,  32,  0,255 }
                  , cP16[]  = { 222,222,222,  16,  0,255 }
                  , cP8[]   = { 255,255,255,   8,  0,255 }
                  , cP1[]   = {  48, 48, 48,   1,  0,255 }
                  ;
  //partition
  {
  //P128
  int i=1,j=128,s;
  image.draw_rectangle(i,0,j,height,cP128);
  cimg_forY(image,y) {s=0;cimg_for_inX(image,i,j,x) image(x,y,0,SP)=++s;}
  //P64
  i=j+1;j+=64;
  image.draw_rectangle(i,0,j,height,cP64);
  cimg_forY(image,y) {s=0;cimg_for_inX(image,i,j,x) image(x,y,0,SP)=++s;}
  //P32
  i=j+1;j+=32;
  image.draw_rectangle(i,0,j,height,cP32);
  cimg_forY(image,y) {s=0;cimg_for_inX(image,i,j,x) image(x,y,0,SP)=++s;}
  //P16
  i=j+1;j+=16;
  image.draw_rectangle(i,0,j,height,cP16);
  cimg_forY(image,y) {s=0;cimg_for_inX(image,i,j,x) image(x,y,0,SP)=++s;}
  //P8
  i=j+1;j+=8;
  image.draw_rectangle(i,0,j,height,cP8);
  cimg_forY(image,y) {s=0;cimg_for_inX(image,i,j,x) image(x,y,0,SP)=++s;}
  //P1
  i=j+1;j+=8;
  image.draw_rectangle(i,0,j,height,cP1);
  cimg_forY(image,y) {s=0;cimg_for_inX(image,i,j,x) image(x,y,0,SP)=++s;}
  }
  //value
  cimg_forXY(image,x,y)
    image(x,y,0,V)=y;
  //partition sum
  image.draw_line(0,0,image.width()-1,0,black);//line
  image.draw_line(0,0,0,image.height()-1,black);//column
  //GUI
  //set other point(s) by mouse
  CImgDisplay disp(image.width()*zoom,image.height()*zoom);
  disp.set_title("(un)select point(s), exit on any key or window closing.");
  while(true)
  {
    //get point
    CImg<int> point=image.get_select(disp,0,0,true);
    int i=point(0);
    int j=point(1);
    //break on key pressed or display close
    if(i==-1||j==-1) break;
    //check point
    if(image(i,j,0,R)==green[R]&&image(i,j,0,G)==green[G]&&image(i,j,0,B)==green[B])//RGB
    {//remove point
      image.draw_point(i,j,black);
      continue;
    }
    //else add point
    CImg<unsigned char>  color(1,1,1,spectrum);color.draw_point(0,0,green);//RGB
    color(P)=(i<129)?128:(i<129+64)?64:(i<129+64+32)?32:(i<129+64+32+16)?16:(i<129+64+32+16+8)?8:1;//Partition
    color(SP)=(i<129)?i:(i<129+64)?i-128:(i<129+64+32)?i-128-64:(i<129+64+32+16)?i-128-64-32:(i<129+64+32+16+8)?i-128-64-32-16:i-128-64-32-16-8;//SubPartition
    color(V)=j;//Value
    image.draw_point(i,j,color.data());
    if(image(i,0,0,R)==black[R]&&image(i,0,0,G)==black[G]&&image(i,0,0,B)==black[B])/*RGB*/ image.draw_point(i,0,color.data());
    else {image.draw_point(i,0,red);image.draw_point(0,0,red);}//sum
    if(image(0,j,0,R)==black[R]&&image(0,j,0,G)==black[G]&&image(0,j,0,B)==black[B])/*RGB*/ image.draw_point(0,j,color.data());
    else {image.draw_point(0,j,red);image.draw_point(0,0,red);}//sum
  }//point selection by hand
  disp.close();
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
//  image.display("LUT");
  image.save("claLUT.png");
  return 0;
}
