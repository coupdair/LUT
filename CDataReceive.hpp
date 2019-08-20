#ifndef _DATA_RECEIVE_
#define _DATA_RECEIVE_

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/shared_ptr.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "UDP/allocator.hpp"

#include <iostream> //std::cout, std::err

using boost::asio::ip::udp;

#include "UDP/yield.hpp"

//! global variable of ASIO service
/**
 * \c io_service is noncopyable, that means you should only refer to it via its address
**/
boost::asio::io_service io_service(1);

#define TIMER_DELAY 543

boost::posix_time::time_duration interval(boost::posix_time::milliseconds(TIMER_DELAY));
boost::asio::deadline_timer timer_(io_service,interval);

void timer_handler(const boost::system::error_code& error);
void timer_init()
{
  timer_.expires_at(timer_.expires_at()+interval);
  timer_.async_wait(timer_handler);
}//timer_init

void timer_handler(const boost::system::error_code& error)
{
  std::cout.flush();
  std::cerr.flush();
  timer_init();
}//timer_handler

#include "UDP/udp_server_class.hpp"

//////////////////////////////////////

#include "UDP/unyield.hpp"

#include "CDataBuffer.hpp"

template<typename Tdata,typename Tdout, typename Taccess=unsigned char>
class CDataReceive : public CDataBuffer<Tdata,Tdout, Taccess>
{
public:

  std::vector<boost::shared_ptr<udp_server> > servers;
  boost::shared_ptr<udp_server> s;
  boost::asio::io_service *io_service;
  std::vector<unsigned char>compare_vector;
  std::vector<unsigned char> rec_buf;

#define TIMER_DELAY 543

  CDataReceive(std::vector<omp_lock_t*> &lock, unsigned short port, int buf_size, boost::asio::io_service *io_service) : CDataBuffer<Tdata, Taccess>(lock)
   , s(new udp_server(*io_service, port, buf_size))
   , io_service(io_service)
  {
    this->debug=true;
    this->class_name="CDataReceiver";
    servers.push_back(s);
    this->check_locks(lock);
  }

  //! copy the data in a vector in a CImg
  void copy2cimg(std::vector<unsigned char> *vec,CImg<Tdata>&image)
  {
    for(unsigned int i=0; i < vec->size(); ++i)
    {
      image[i]=static_cast<Tdata>((*vec)[i]);
    }
  }//copy2cimg

  virtual void iteration(CImg<unsigned char> &access,CImgList<Tdata> &images, int n, int i)
  {
    if(this->debug)
    {
      this->lprint.print("",false);
      printf("4 B%02d #%04d: ",n,i);fflush(stdout);
      access.print("access",false);fflush(stderr);
      this->lprint.unset_lock();
    }

    //wait lock
    unsigned int c=0;
    this->laccess.wait_for_status(access[n],this->STATUS_FREE,this->STATE_RECEIVING, c);//free, receiving

//! \todo [low] receive buffer could be before lock
    //spin
    rec_buf.clear();
    while(rec_buf.size()==0)
    {
      //Getting the info in the buffer
      (*s)(boost::system::error_code(), &rec_buf);
      io_service->poll();
      if(compare_vector==rec_buf)
        rec_buf.clear();
      else
        compare_vector=rec_buf;
    }//while

    //copy buffer to circular buffer
    copy2cimg(&rec_buf,images[n]);

    this->laccess.set_status(access[n],this->STATE_RECEIVING,this->STATUS_RECEIVED, this->class_name[5],i,n,c);//receiving, received
  }//iteration

};//CDataReceive

#endif //_DATA_RECEIVE_

