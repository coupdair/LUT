all: nogui
	
gui: main.cpp thread_lock.hpp
	g++ -O0 -o generate.X main.cpp -I../CImg -lboost_system -Wall -W -ansi -pedantic -Dcimg_use_vt100 -I/usr/X11R6/include  -lm -L/usr/X11R6/lib -lpthread -fopenmp -lX11 && ./generate.X -h -I && ./generate.X -v > VERSION
	./generate.X -h 2> generateX_help.output

nogui: main.cpp thread_lock.hpp
	g++ -O0 -o generate   main.cpp -I../CImg -lboost_system -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -fopenmp -Dcimg_display=0 && ./generate -h -I && ./generate -v > VERSION
	./generate -h 2> generate_help.output

run:
	./generate -c 2 -s 4096 -b   6 -n 12

clear:
	rm sample_??????.cimg

clean:
	rm generate.X generate sample_??????.cimg

