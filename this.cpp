// derived classes
#include <iostream>
using namespace std;

template<typename T>
class Polygon {
  public:
    T width, height;
    void set_values (int a, int b)
      { width=a; height=b;}
 };

template<typename T>
class Rectangle: public Polygon<T> {
  public:
    T w;
    T area ()
      { return width * height; }
 };

template<typename T>
class Square: public Rectangle<T> {
  public:
    void set_values (T a)
      { w=a; width=a; height=a;}
 };

template<typename T>
class Triangle: public Polygon<T> {
  public:
    T area ()
      { return width * height / 2; }
  };
  
int main ()
{
  Rectangle<int> rect;
  Square<int> square;
  Triangle<int> trgl;
  rect.set_values (4,5);
  square.set_values (4);
  trgl.set_values (4,5);
  cout << rect.area() << '\n';
  cout << square.area() << '\n';
  cout << trgl.area() << '\n';
  return 0;
}//main

