#ifndef _DATA_SENDER_
#define _DATA_SENDER_

#include <boost/asio/ip/udp.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "UDP/high_res_clock.hpp"

using boost::asio::ip::udp;
using boost::posix_time::ptime;
using boost::posix_time::microsec_clock;

#include "CDataAccess.hpp"

class CDataSender : CDataAccess
{
public:

  boost::asio::io_service io_service;
  udp::socket socket;
  udp::endpoint target;

  CDataSender(std::vector<omp_lock_t*> &lock, std::string ip, unsigned short port) : CDataAccess(lock), socket(io_service, udp::endpoint(udp::v4(), 0)), target(boost::asio::ip::address::from_string(ip), port)
  {
    debug=true;
    class_name="CDataSender";
    if(lock.size()<2)
    {
      printf("code error: locks should have at least 2 locks for %s class.",class_name.c_str());
      exit(99);
    }
    udp::socket::non_blocking_io nbio(true);
    socket.io_control(nbio);
    target.port(port);
  }//constructor

  virtual void iteration(CImg<unsigned char> &access, std::vector<unsigned char> write_buf, int n, int i, boost::uint64_t wait)
  {
    if(debug)
    {
      lprint.print("",false);
      printf("4 B%02d #%04d: ",n,i);fflush(stdout);
      access.print("access",false);fflush(stderr);
      lprint.unset_lock();
    }
    //wait lock
    unsigned int c=0;
    laccess.wait_for_status(access[n],STATUS_FILLED,STATE_SENDING, c);//filled, sending

    //send data by udp
    boost::system::error_code ec;
    socket.send_to(boost::asio::buffer(write_buf), target, 0, ec);
    boost::uint64_t time_hr = high_res_clock();
    while ((high_res_clock()-time_hr)<wait) {}

    //set free
    laccess.set_status(access[n],STATE_SENDING,STATUS_FREE, class_name[5],i,n,c);//sent, now free

    //std::cout << std::endl << std::endl << i << " sent" << std::endl;
  }//iteration
};//CDataSender

#endif
