#run
DATA=./
DATA=/media/temp/
DIN=samples/
DOUT=results/
FIN=sample.png
FOUT=$(FIN)
UDP_SIZE=4096

#compiler options
LIB_XWINDOWS=-I/usr/X11R6/include -L/usr/X11R6/lib -lX11
LIB_CIMG=-I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -lm -fopenmp
LIB_BOOST_ASIO=-lboost_system
LIB_BOOST_COMPUTE=-lMali -L/usr/lib/aarch64-linux-gnu/ -DBOOST_COMPUTE_MAX_CL_VERSION=102

#source package
#TODO
all: process

gui: main.cpp
	g++ -O0 -o generate.X main.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -lm -fopenmp -lboost_system $(LIB_XWINDOWS) && ./generate.X -h -I && ./generate.X -v > VERSION
	./generate.X -h 2> generateX_help.output

process: process.cpp thread_lock.hpp CDataAccess.hpp CDataBuffer.hpp CDataGenerator.hpp CDataStore.hpp
	g++ -O0 -o process   process.cpp $(LIB_CIMG) -Dcimg_display=0 && ./process -h -I && ./process -v > VERSION
	./process -h 2> process_help.output

send: send.cpp thread_lock.hpp CDataAccess.hpp CDataBuffer.hpp CDataGenerator.hpp CDataSend.hpp
	g++ -O0 -o send   send.cpp  $(LIB_CIMG) $(LIB_BOOST_ASIO) -Dcimg_display=0 && ./send -h -I && ./send -v > VERSION
	./send -h 2> send_help.output

receive: receive.cpp thread_lock.hpp CDataAccess.hpp CDataBuffer.hpp CDataStore.hpp CDataReceive.hpp CDataProcessor.hpp CDataProcessorGPU.hpp
	g++ -O0 -o receive receive.cpp  $(LIB_CIMG) $(LIB_BOOST_ASIO) -Dcimg_display=0 $(LIB_BOOST_COMPUTE) && ./receive -h -I && ./receive -v > VERSION
	./receive -h 2> receive_help.output

send_run:
	#./send -c 2 -s 12500 -b 12 -n 409600 -w 250000
	./send    -c 2 -s $(UDP_SIZE) -b  8 -n 256 -w 234567890

receive_run:
	mkdir -p $(DATA)$(DIN)  $(DATA)$(DOUT)
	rm -f    $(DATA)$(DIN)* $(DATA)$(DOUT)*
	./receive -c 3 -s $(UDP_SIZE) -b 16 -n 255 -o $(DATA)$(DIN)$(FIN) -r $(DATA)$(DOUT)$(FOUT)

clear:
	rm -f $(DATA)/samples/* $(DATA)/results/*
	rm -f sample_??????.cimg

clean: clear
	rm -f send.X    send
	rm -f receive.X receive
	rm -f process.X process

display:
	convert -append $(DATA)/samples/sample*.png $(DATA)/samples.png && display samples.png &
	convert -append $(DATA)/results/sample*.png $(DATA)/results.png && display results.png

