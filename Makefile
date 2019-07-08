all: gui nogui
	
gui: storer.cpp
	g++ -O0 -o store.X storer.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -I/usr/X11R6/include  -lm -L/usr/X11R6/lib -lpthread -fopenmp -lX11 && ./store.X -h -I && ./store.X -v > VERSION
nogui: storer.cpp
	g++ -O0 -o store   storer.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -fopenmp -Dcimg_display=0 && ./store -h -I && ./store -v > VERSION

