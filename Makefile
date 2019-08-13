all: generate receive

gui: main.cpp
	g++ -O0 -o generate.X main.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -lm -fopenmp -lboost_system -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 && ./generate.X -h -I && ./generate.X -v > VERSION
	./generate.X -h 2> generateX_help.output

send: send.cpp thread_lock.hpp CDataAccess.hpp CDataGenerator.hpp CDataSend.hpp
	g++ -O0 -o send   send.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -lm -fopenmp -lboost_system -Dcimg_display=0 && ./send -h -I && ./send -v > VERSION
	./send -h 2> send_help.output

receive: receive.cpp thread_lock.hpp CDataAccess.hpp CDataStore.hpp CDataReceive.hpp CDataProcessor.hpp
	g++ -O0 -o receive receive.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -lm -fopenmp -lboost_system -Dcimg_display=0 -lMali -L/usr/lib/aarch64-linux-gnu/ -DBOOST_COMPUTE_MAX_CL_VERSION=102 && ./receive -h -I && ./receive -v > VERSION
	./receive -h 2> receive_help.output

send_run:
	#./send -c 2 -s 12500 -b 12 -n 409600 -w 250000
	./send -c 2 -s 1024 -b 12 -n 256 -w 234567890

receive_run:
	mkdir -p samples/ addition/
	rm -f samples/*
	./receive -c 3 -s 1024 -b   15 -n 256 -p 1234

clear:
	rm -f samples/* addition/*

clean:
	rm -f receive.X receive samples/* addition/*
	rm -f send.X send

display:
	convert -append samples/sample*.png result.png && display result.png &
	convert -append addition/Asample*.png result_add.png && display result_add.png

