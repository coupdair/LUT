# description

TODO

# compile

## external library

- [`CImg`](http://www.cimg.eu/)
     - XWindows via `X11` development library for window output

~~~ { .bash }
sudo apt-get install cimg-dev cimg-doc cimg-examples libxrandr-dev
~~~

- [`ImageMagick`](http://imagemagick.org) via `CImg`
     - `convert` for PNG output

~~~ { .bash }
sudo apt-get install imagemagick
~~~

- or [`libPNG`](http://libpng.org/pub/png) via `CImg`
     - embedded PNG output, need compilation option.

~~~ { .bash }
sudo apt-get install libpng-dev
~~~

## git clone

git for both `CImg` and `claLUT`

~~~ { .bash }
git clone https://github.com/dtschump/CImg.git
git clone git@github.com:coupdair/claLUT.git
#or
git clone https://github.com/dtschump/CImg.git
git clone https://github.com/coupdair/claLUT.git
~~~

## C++ compilation

cf. [`_info.txt`](_info.txt)

e.g.

~~~ { .bash }
make
~~~

# use

TODO

