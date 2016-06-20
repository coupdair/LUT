# Check and Layout A LookUpTable

both check the validity and show the layout of a LUT.

- Layout the LUT as a matrix (i.e. kind of digital 2D plot).
- Check columns and line for duplicates, i.e. errors in setup.

c est la LUT, est-elle correcte ou non ?!


# compile

## external library

- `CImg`
     - XWindows via `X11` development library
- `ImageMagick`
     - `convert` for PNG
- or `libPNG`

## C++ compilation

cf. `_info.txt`

e.g.

~~~ { .bash }
g++ -O0 -o claLUT claLUT.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -I/usr/X11R6/include  -lm -L/usr/X11R6/lib -lpthread -lX11 
~~~

# use

run binary: `claLUT`
check results on pop up window or `claLUT.png` file.

e.g.

~~~ { .bash }
#run with window output
./claLUT
#image output
display claLUT.png
~~~


