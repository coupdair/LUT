// derived classes
#include <iostream>
using namespace std;

class Polygon
{
  public:
    int width, height;
    void set_values (int a, int b)
      { width=a; height=b;}
};//Polygon

template<typename T>
class Rectangle: public Polygon
{
  public:
    T w;
    int area ()
      { return width * height; }
};//Rectangle

template<typename T>
class Square: public Rectangle<T>
{
  public:
    void set_values (int a)
      { this->w=a; this->width=a; this->height=a;}
};//Square

class Triangle: public Polygon
{
  public:
    int area ()
      { return width * height / 2; }
};//Triangle
  
int main ()
{
  Rectangle<int> rect;
  Square<int> square;
  Triangle trgl;
  rect.set_values (4,5);
  square.set_values (4);
  trgl.set_values (4,5);
  cout << rect.area() << '\n';
  cout << square.area() << '\n';
  cout << trgl.area() << '\n';
  return 0;
}//main

