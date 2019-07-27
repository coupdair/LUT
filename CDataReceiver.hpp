#ifndef _DATA_RECEIVER_
#define _DATA_RECEIVER_

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

#include "UDP/udp_server_class.hpp"

#include "CDataAccess.hpp"

class CDataReceiver : CDataAccess
{
public:

  std::vector<boost::shared_ptr<udp_server> > servers;
  boost::shared_ptr<udp_server> s;
  std::vector<unsigned char>compare_vector;

#define TIMER_DELAY 543

  CDataReceiver(std::vector<omp_lock_t*> &lock, unsigned short port, int buf_size, boost::asio::io_service *io_service) : CDataAccess(lock), s(new udp_server(*io_service, port, buf_size))
  {
    debug=true;
    class_name="CDataReceiver";
    if(lock.size()<2)
    {
      printf("code error: locks should have at least 2 locks for %s class.",class_name.c_str());
      exit(99);
    }
    servers.push_back(s);
  }

  virtual void iteration(CImg<unsigned char> &access, std::vector<unsigned char> rec_buf, int n, int i, boost::asio::io_service *io_service)
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
    laccess.wait_for_status(access[n],STATUS_FREE,STATE_RECEIVING, c);//free, receiving

    rec_buf.clear();


    //if(spin)
    //définition en spin par défaut, block n'est pas adapté pour la programmation de processus parallèles
    while(rec_buf.size()==0)
    {
      //Getting the info in the buffer
      (*s)(boost::system::error_code(), &rec_buf);
      //std::cout << "rec_buf.size() : " << rec_buf.size() << std::endl;
      /*if(rec_buf.size()!=0)
      {
        std::cout << std::endl << "Size : " << rec_buf.size() << " / value in CDataReceiver : " << rec_buf[0] << std::endl;
      }*/
      io_service->poll();
      if(compare_vector==rec_buf)
      {
	rec_buf.clear();
      }
      else
        compare_vector=rec_buf;
    }
    std::cout<<std::endl<<rec_buf[0]<<" received"<<std::endl;

    laccess.set_status(access[n],STATE_RECEIVING,STATUS_RECEIVED, class_name[5],i,n,c);//receiving, received

  }//iteration
};//CDataReceiver

#endif
