#run
DATA=./
DATA=/media/temp/
UDP_SIZE=4096


all: send

gui: main.cpp
	g++ -O0 -o generate.X main.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -lm -fopenmp -lboost_system -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 && ./generate.X -h -I && ./generate.X -v > VERSION
	./generate.X -h 2> generateX_help.output

send: send.cpp thread_lock.hpp CDataAccess.hpp CDataBuffer.hpp CDataGenerator.hpp CDataSend.hpp
	g++ -O0 -o send   send.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -lm -fopenmp -lboost_system -Dcimg_display=0 && ./send -h -I && ./send -v > VERSION
	./send -h 2> send_help.output

receive: receive.cpp thread_lock.hpp CDataAccess.hpp CDataBuffer.hpp CDataStore.hpp CDataReceive.hpp CDataProcessor.hpp
	g++ -O0 -o receive receive.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -lm -fopenmp -lboost_system -Dcimg_display=0 -lMali -L/usr/lib/aarch64-linux-gnu/ -DBOOST_COMPUTE_MAX_CL_VERSION=102 && ./receive -h -I && ./receive -v > VERSION
	./receive -h 2> receive_help.output

send_run:
	#./send -c 2 -s 12500 -b 12 -n 409600 -w 250000
	./send    -c 2 -s $(UDP_SIZE) -b  8 -n 256 -w 234567890

receive_run:
	mkdir -p $(DATA)/samples/  $(DATA)/results/
	rm -f    $(DATA)/samples/* $(DATA)/results/*
	./receive -c 3 -s $(UDP_SIZE) -b 16 -n 255 -p 1234

clear:
	rm -f $(DATA)/samples/* $(DATA)/results/*

clean:
	rm -f receive.X receive $(DATA)/samples/* $(DATA)/results/*
	rm -f send.X send

display:
	convert -append $(DATA)/samples/sample*.png $(DATA)/samples.png && display samples.png &
	convert -append $(DATA)/results/sample*.png $(DATA)/results.png && display results.png

