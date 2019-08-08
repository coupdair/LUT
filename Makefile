all: nogui

gui: main.cpp thread_lock.hpp
	g++ -O0 -o store.X main.cpp -std=c++03 -I../CImg -lboost_system -lboost_thread -Wall -W -ansi -pedantic -Dcimg_use_vt100 -I/usr/X11R6/include  -lm -L/usr/X11R6/lib -lpthread -fopenmp -lX11 -lMali -L/usr/lib/aarch64-linux-gnu/ -DBOOST_COMPUTE_MAX_CL_VERSION=102 && ./store.X -h -I && ./store.X -v > VERSION
	./store.X -h 2> storeX_help.output

nogui: main.cpp thread_lock.hpp
	g++ -O0 -o store   main.cpp -std=c++03 -I../CImg -lboost_system -lboost_thread -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -fopenmp -Dcimg_display=0 -lMali -L/usr/lib/aarch64-linux-gnu/ -DBOOST_COMPUTE_MAX_CL_VERSION=102 && ./store -h -I && ./store -v > VERSION
	./store -h 2> store_help.output

run:
	rm samples/*
	./store -c 3 -s 1024 -b   15 -n 256 -p 1234

clear:
	rm samples/*

clean:
	rm store.X store samples/*

display:
	convert -append samples/sample*.png result.png && display result.png &
	convert -append addition/Asample*.png result_add.png && display result_add.png && display result_add.png
