// derived classes
#include <iostream>
using namespace std;

class Polygon {
  public:
    int width, height;
    void set_values (int a, int b)
      { width=a; height=b;}
 };

class Rectangle: public Polygon {
  public:
    int area ()
      { return width * height; }
 };

template<typename T>
class Square: public Rectangle {
  public:
    T w;
    void set_values (int a)
      { w=a; width=a; height=a;}
 };

class Triangle: public Polygon {
  public:
    int area ()
      { return width * height / 2; }
  };
  
int main ()
{
  Rectangle rect;
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

