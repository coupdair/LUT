all: nogui
	
gui: main.cpp thread_lock.hpp
	g++ -O0 -o store.X main.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -I/usr/X11R6/include  -lm -L/usr/X11R6/lib -lpthread -fopenmp -lX11 -std=c++11 && ./store.X -h -I && ./store.X -v > VERSION
	./store.X -h 2> storeX_help.output

nogui: main.cpp thread_lock.hpp
	g++ -O0 -o store   main.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -fopenmp -Dcimg_display=0 -std=c++11 && ./store -h -I && ./store -v > VERSION
	./store -h 2> store_help.output

run:
	./store -c 2 -s 4096 -b   6 -n 12

clear:
	rm sample_??????.cimg

clean:
	rm store.X store sample_??????.cimg

