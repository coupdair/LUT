//
// udp_server.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/shared_ptr.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "allocator.hpp"

#include <iostream> //std::cout, std::cerr

using boost::asio::ip::udp;

#include "yield.hpp"

//! \todo need timer for following operations:
//! \todo need to check and count
//! \todo need to flush, e.g. "std::cout<<std::flush;"

  boost::asio::io_service io_service(1);
  //! timer
#define TIMER_DELAY 543
  boost::posix_time::time_duration interval(boost::posix_time::milliseconds(TIMER_DELAY));
  boost::asio::deadline_timer timer_(io_service,interval);
//timer looping
void timer_handler(const boost::system::error_code& error);
void timer_init() {
   timer_.expires_at(timer_.expires_at()+interval);
   timer_.async_wait(timer_handler);
}//timer_init
void timer_handler(const boost::system::error_code& error) {
//   static long count=0;
//   std::cout << "in timer_handler " << count++ << std::endl;
   std::cout.flush();
   std::cerr.flush();
   //start timer
   timer_init();
}//timer_handler

class udp_server : coroutine
{
public:
  udp_server(boost::asio::io_service& io_service,
      unsigned short port, std::size_t buf_size) :
    socket_(io_service, udp::endpoint(udp::v4(), port)),
    buffer_(buf_size)
//    ,interval(boost::posix_time::milliseconds(250)), timer_(io_service,interval)
  {
    count_=0;

  }

  void operator()(boost::system::error_code ec, std::size_t n = 0)
  {
    //infinite loop for receive (and send back)
    reenter (this) for (;;)
    {
      yield socket_.async_receive_from(
          boost::asio::buffer(buffer_),
          sender_, ref(this));
      if (!ec)
      {
        count_++;
        //reading index as first 64bit of frame
        boost::uint64_t *p_index=(boost::uint64_t*)(buffer_.data());
        boost::uint64_t index=*p_index;
        std::printf("receive   #%d as #%d\r",index,count_);
//        std::printf("receive   #%d size=%d\r",count_,buffer_.size());std::cout<<std::flush;
        //change buffer
//        for (std::size_t i = 0; i < n; ++i) buffer_[i] = ~buffer_[i];
        //std::printf("send back\r");std::cout<<std::flush;
        //send back
//        socket_.send_to(boost::asio::buffer(buffer_, n), sender_, 0, ec);
      }
    }//infinite loop
  }

  friend void* asio_handler_allocate(std::size_t n, udp_server* s)
  {
    return s->allocator_.allocate(n);
  }

  friend void asio_handler_deallocate(void* p, std::size_t, udp_server* s)
  {
    s->allocator_.deallocate(p);
  }

  struct ref
  {
    explicit ref(udp_server* p)
      : p_(p)
    {
    }

    void operator()(boost::system::error_code ec, std::size_t n = 0)
    {
      (*p_)(ec, n);
    }

  private:
    udp_server* p_;

    friend void* asio_handler_allocate(std::size_t n, ref* r)
    {
      return asio_handler_allocate(n, r->p_);
    }

    friend void asio_handler_deallocate(void* p, std::size_t n, ref* r)
    {
      asio_handler_deallocate(p, n, r->p_);
    }
  };//ref

private:
  udp::socket socket_;
  std::vector<unsigned char> buffer_;
  udp::endpoint sender_;
  allocator allocator_;
  //! count received
  unsigned int count_;
};//udp_server

#include "unyield.hpp"

int main(int argc, char* argv[])
{
  if (argc != 5)
  {
    std::fprintf(stderr,
        "Usage: udp_server <port1> <nports> "
        "<bufsize> {spin|block}\n");
    return 1;
  }

  unsigned short first_port = static_cast<unsigned short>(std::atoi(argv[1]));
  unsigned short num_ports = static_cast<unsigned short>(std::atoi(argv[2]));
  std::size_t buf_size = std::atoi(argv[3]);
  bool spin = (std::strcmp(argv[4], "spin") == 0);

  std::vector<boost::shared_ptr<udp_server> > servers;

  //start timer
  std::fprintf(stderr,"timer interval=%d ms\n",TIMER_DELAY);std::cerr<<std::flush;
  timer_.async_wait(timer_handler);

  //launch several UDP servers by incrementing port
  for (unsigned short i = 0; i < num_ports; ++i)
  {
    unsigned short port = first_port + i;
    boost::shared_ptr<udp_server> s(new udp_server(io_service, port, buf_size));
    servers.push_back(s);
    (*s)(boost::system::error_code());
  }//port loop

  if (spin)
    for (;;) io_service.poll();
  else
    io_service.run();
}//main

