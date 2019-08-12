all: nogui
	
gui: main.cpp thread_lock.hpp
	g++ -O0 -o generate.X main.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -lm -fopenmp -lboost_system -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 && ./generate.X -h -I && ./generate.X -v > VERSION
	./generate.X -h 2> generateX_help.output

nogui: main.cpp thread_lock.hpp
	g++ -O0 -o generate   main.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -lm -fopenmp -lboost_system -Dcimg_display=0 && ./generate -h -I && ./generate -v > VERSION
	./generate -h 2> store_help.output

run:
	#./generate -c 2 -s 12500 -b 12 -n 409600 -w 250000
	./generate -c 2 -s 1024 -b 12 -n 256 -w 234567890

clean:
	rm generate.X generate

