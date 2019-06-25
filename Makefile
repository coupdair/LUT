all: gui nogui
	
gui: claLUT.cpp
	g++ -O0 -o claLUT.X claLUT.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -I/usr/X11R6/include  -lm -L/usr/X11R6/lib -lpthread -lX11 && ./claLUT.X -h -I && ./claLUT.X -v > VERSION
nogui: claLUT.cpp
	g++ -O0 -o claLUT   claLUT.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -Dcimg_display=0 && ./claLUT -h -I && ./claLUT -v > VERSION
