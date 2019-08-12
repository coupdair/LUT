all: udp_server udp_client
	echo "make server and client"

udp_server:  udp_server.cpp allocator.hpp high_res_clock.hpp coroutine.hpp yield.hpp unyield.hpp
	g++ udp_server.cpp -o udp_server -std=c++03 -I /usr/include/boost/ -lboost_system -lboost_thread -pthread

udp_client:  udp_client.cpp allocator.hpp high_res_clock.hpp coroutine.hpp yield.hpp unyield.hpp
	g++ udp_client.cpp -o udp_client -std=c++03 -I /usr/include/boost/ -lboost_system -lboost_thread -pthread

