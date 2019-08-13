all: nogui

gui: main.cpp
	g++ -O0 -o generate.X main.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -lm -fopenmp -lboost_system -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 && ./generate.X -h -I && ./generate.X -v > VERSION
	./generate.X -h 2> generateX_help.output

nogui: main.cpp thread_lock.hpp CDataAccess.hpp CDataGenerator.hpp CDataStore.hpp
	g++ -O0 -o store   main.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -lm -fopenmp -lboost_system -Dcimg_display=0 && ./store -h -I && ./store -v > VERSION
	./store -h 2> store_help.output

run:
	mkdir samples/ addition/
	rm samples/*
	./store -c 3 -s 1024 -b   15 -n 256 -p 1234

clear:
	rm samples/*

clean:
	rm store.X store samples/*

display:
	convert -append samples/sample*.png result.png && display result.png &
	convert -append addition/Asample*.png result_add.png && display result_add.png

