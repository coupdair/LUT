#ifndef _DATA_SEND_
#define _DATA_SEND_

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

template<typename Tdata, typename Taccess=unsigned char>
class CDataSend : CDataAccess
{
public:

  boost::asio::io_service io_service;
  udp::socket socket;
  udp::endpoint target;
  boost::uint64_t wait;
  std::vector<unsigned char> write_buf;

  CDataSend(std::vector<omp_lock_t*> &lock, std::string ip, unsigned short port, boost::uint64_t wait) : CDataAccess(lock)
    , socket(io_service, udp::endpoint(udp::v4(), 0))
    , target(boost::asio::ip::address::from_string(ip), port)
    , wait(wait)
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

  //! copy the data in a CImg in a vector
  //! \todo [medium] CImg<T> to vector<T>, try also if send with ASIO works
  void copy2vector(CImg<unsigned int> img)
  {
    for(int i=0; i<img.width(); ++i)
    {
      write_buf.push_back(static_cast<unsigned char>(img[i]));
    }
  }//copy2vector

  virtual void iteration(CImg<Taccess> &access,CImgList<Tdata> &images, int n, int i)
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

  }//iteration

  //! run for loop
  virtual void run(CImg<Taccess> &access,CImgList<Tdata> &images, unsigned int count)
  {
    unsigned int nbuffer=images.size();
    for(unsigned int n=0,i=0;i<count;++i,++n)
    {
      //! \todo [high] move copy2vector to iteration, consequently suppress run that should inherit from CDataBuffer
      copy2vector(images[n]);
      this->iteration(access,images, n,i);
      //circular buffer
       if(n==nbuffer-1) n=-1;
    }//vector loop
  }//run

};//CDataSend

#endif //_DATA_SEND_

