all: gui nogui
	
gui: generator.cpp
	g++ -O0 -o generate.X generator.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -I/usr/X11R6/include  -lm -L/usr/X11R6/lib -lpthread -fopenmp -lX11 && ./generate.X -h -I && ./generate.X -v > VERSION
nogui: generator.cpp
	g++ -O0 -o generate   generator.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -fopenmp -Dcimg_display=0 && ./generate -h -I && ./generate -v > VERSION

