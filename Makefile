#run
FRAME_SIZE=4096

DATA=./
DATA=/media/temp/
DIN=samples/
DOUT=results/
FIN=sample.png
FOUT=$(FIN)

#compiler options
LIB_XWINDOWS=-I/usr/X11R6/include -L/usr/X11R6/lib -lX11
LIB_CIMG=-I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -lm -fopenmp
LIB_BOOST_ASIO=-lboost_system
LIB_BOOST_COMPUTE=-lMali -L/usr/lib/aarch64-linux-gnu/ -DBOOST_COMPUTE_MAX_CL_VERSION=102

DO_GPU=-DDO_GPU $(LIB_BOOST_COMPUTE)
#DO_GPU=

#source package
SRC_DATA_BUFFER=thread_lock.hpp CDataAccess.hpp CDataBuffer.hpp

all: process

gui: main.cpp
	g++ -O0 -o generate.X main.cpp -I../CImg -Wall -W -ansi -pedantic -Dcimg_use_vt100 -lpthread -lm -fopenmp -lboost_system $(LIB_XWINDOWS) && ./generate.X -h -I && ./generate.X -v > VERSION
	./generate.X -h 2> generateX_help.output

process: process.cpp $(SRC_DATA_BUFFER) CDataGenerator.hpp CDataProcessor.hpp CDataProcessorGPU.hpp CDataStore.hpp
	g++ -O0 -o process   process.cpp $(LIB_CIMG) -Dcimg_display=0 $(DO_GPU) && ./process -h -I && ./process -v > VERSION
	./process -h 2> process_help.output

send: send.cpp $(SRC_DATA_BUFFER) CDataGenerator.hpp CDataSend.hpp
	g++ -O0 -o send   send.cpp  $(LIB_CIMG) $(LIB_BOOST_ASIO) -Dcimg_display=0 && ./send -h -I && ./send -v > VERSION
	./send -h 2> send_help.output

receive: receive.cpp $(SRC_DATA_BUFFER)  CDataStore.hpp CDataReceive.hpp CDataProcessor.hpp CDataProcessorGPU.hpp
	g++ -O0 -o receive receive.cpp  $(LIB_CIMG) $(LIB_BOOST_ASIO) -Dcimg_display=0 $(DO_GPU) && ./receive -h -I && ./receive -v > VERSION
	./receive -h 2> receive_help.output

process_run:
	./process -c 3 -s $(FRAME_SIZE)

send_run:
	./send    -c 2 -s $(FRAME_SIZE) -b  8 -n 256 -w 76543210

receive_run:
	mkdir -p $(DATA)$(DIN)  $(DATA)$(DOUT)
	rm -f    $(DATA)$(DIN)* $(DATA)$(DOUT)*
	./receive -c 4 -s $(FRAME_SIZE) -b 16 -n 254 -o $(DATA)$(DIN)$(FIN) -r $(DATA)$(DOUT)$(FOUT)

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

