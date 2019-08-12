all: nogui
	
gui: main.cpp thread_lock.hpp
	g++ -O0 -o store.X main.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -lm -fopenmp -lboost_system -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 && ./store.X -h -I && ./store.X -v > VERSION
	./store.X -h 2> storeX_help.output

nogui: main.cpp thread_lock.hpp
	g++ -O0 -o store   main.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -lm -fopenmp -lboost_system -Dcimg_display=0 && ./store -h -I && ./store -v > VERSION
	./store -h 2> store_help.output

run:
	./store -c 2 -s 4096 -b   6 -n 12

clear:
	rm sample_??????.cimg

clean:
	rm store.X store sample_??????.cimg

