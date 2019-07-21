//
// udp_client.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/asio/ip/udp.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <vector>
#include "high_res_clock.hpp"

//random
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

using boost::asio::ip::udp;
using boost::posix_time::ptime;
using boost::posix_time::microsec_clock;

int main(int argc, char* argv[])
{
  if (argc != 9)
  {
    std::fprintf(stderr,
        "Usage: udp_client <ip>  <port1> <nports> "
        "<bufsize> {spin|block} <frame_count> <wait_delay> <random_seeder>\n"
        "e.g. ./udp_client 10.10.15.1  1234567 1  4096 block 100000  2345678 3210\n");
    return 1;
  }

  //CLI
  const char* ip = argv[1];
  unsigned short first_port = static_cast<unsigned short>(std::atoi(argv[2]));
  unsigned short num_ports = static_cast<unsigned short>(std::atoi(argv[3]));
  std::size_t buf_size = static_cast<std::size_t>(std::atoi(argv[4]));
  const bool spin = (std::strcmp(argv[5], "spin") == 0);
  const unsigned long int num_samples = static_cast<unsigned int>(std::atol(argv[6]));
  const boost::uint64_t wait=static_cast<std::size_t>(std::atoi(argv[7]));
  const boost::uint64_t seed=static_cast<std::size_t>(std::atoi(argv[8]));

  //CLI convert show
  if(false)//true or default false
  {
    std::printf(" buf_size=%ld\n",   buf_size);
    std::printf(" num_samples=%lu\n", num_samples);
    std::printf(" wait=%ld\n",       wait);
//    std::printf(" =%d\n",);
  }//CLI convert

  boost::asio::io_service io_service;

  udp::socket socket(io_service, udp::endpoint(udp::v4(), 0));

  if (spin)
  {
    udp::socket::non_blocking_io nbio(true);
    socket.io_control(nbio);
  }

  udp::endpoint target(boost::asio::ip::address::from_string(ip), first_port);
  unsigned short last_port = first_port + num_ports - 1;
  std::vector<unsigned char> write_buf(buf_size);
  {//random fill
  boost::random::mt19937 gen(seed);
  boost::random::uniform_int_distribution<> dist(0,255);
  for(int i=0;i<write_buf.size();++i) write_buf[i]=dist(gen);
  //show random
  std::printf("write_buf[]=");for(int i=0;i<5;++i) std::printf("%d,",write_buf[i]); std::printf("...\n");
  }//random fill
  std::vector<unsigned char> read_buf(buf_size);

  //Same start date of the program with two different clocks, i.e. [us],[]
  ptime start = microsec_clock::universal_time();
  boost::uint64_t start_hr = high_res_clock();

  //loop over samples to send (and receive time, for latency) and port loop too
  boost::uint64_t samples[num_samples];
  for (int i = 0; i < num_samples; ++i)
  {
    //std::printf(" send #%d\r",i);
//    std::printf("send #%d\r",i);std::cout<<std::flush;
    boost::uint64_t t = high_res_clock();

    boost::system::error_code ec;
    //writing index as first 64bit of frame
    boost::uint64_t *index=(boost::uint64_t*)(write_buf.data());
    *index=i;
    socket.send_to(boost::asio::buffer(write_buf), target, 0, ec);

    //std::printf("wait\r");std::cout<<std::flush;
//    do socket.receive(boost::asio::buffer(read_buf), 0, ec);
//    while (ec == boost::asio::error::would_block);

    //compute latency
    samples[i] = high_res_clock() - t;

    //wait (* running)
    boost::uint64_t time_hr = high_res_clock();
    char *temp="|/-\\*";int o=0;
    while ((high_res_clock()-time_hr)<wait) {}//std::printf("%c\r",temp[o++]);o=(o<4)?o:0;std::cout<<std::flush;}

    if (target.port() == last_port)
      target.port(first_port);
    else
      target.port(target.port() + 1);
  }//sample loop

  std::printf("\ndata statistics\nsort %%\tlatency (us)\trate (MB/s)\n");std::cout<<std::flush;
  //Stop of the two clocks, i.e. [us],[]
  ptime stop = microsec_clock::universal_time();
  boost::uint64_t stop_hr = high_res_clock();
  boost::uint64_t elapsed_usec = (stop - start).total_microseconds();
  boost::uint64_t elapsed_hr = stop_hr - start_hr;
  //clock scaling, i.e. high resolution scale in [us]/[]
  double scale = 1.0 * elapsed_usec / elapsed_hr;

  //display latency statistics
  std::sort(samples, samples + num_samples);
  double l=samples[0] * scale;                     std::printf("  0.0%%\t%f\t%f\n",   l,buf_size/l);
  l=samples[num_samples / 1000 - 1] * scale;       std::printf("  0.1%%\t%f\t%f\n",   l,buf_size/l);
  l=samples[num_samples / 100 - 1] * scale;        std::printf("  1.0%%\t%f\t%f\n",   l,buf_size/l);
  for(int i=1;i<10;++i){                                       //10% to 90%
  l=samples[num_samples * i / 10 - 1] * scale;     std::printf(" %d0.0%%\t%f\t%f\n",i,l,buf_size/l);}
  l=samples[num_samples * 99 / 100 - 1] * scale;   std::printf(" 99.0%%\t%f\t%f\n",   l,buf_size/l);
  l=samples[num_samples * 999 / 1000 - 1] * scale; std::printf(" 99.9%%\t%f\t%f\n",   l,buf_size/l);
  l=samples[num_samples - 1] * scale;              std::printf("100.0%%\t%f\t%f\n",   l,buf_size/l);

  double total = 0.0;
  for (int i = 0; i < num_samples; ++i) total += samples[i] * scale;
  l=total / num_samples;
  std::printf("  mean\t%lf\t%lf\n", l,buf_size/l);

  std::printf("elapsed\t%ld s\t%lf MB/s\n", elapsed_usec/1000000, 1.0d*num_samples*buf_size/elapsed_usec);

}//main

