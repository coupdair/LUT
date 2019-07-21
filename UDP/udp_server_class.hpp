#ifndef _TIMER_FUNCTIONS_
#define _TIMER_FUNCTIONS_


#include "yield.hpp"
#include <boost/system/system_error.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "allocator.hpp"

using boost::asio::ip::udp;

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

  void operator()(boost::system::error_code ec, std::vector<unsigned char> rec_buf, std::size_t n = 0)
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
        //std::printf("receive   #%d as #%d\r",index,count_);
        //std::cout << int(buffer_[0]) << " " << int(buffer_[20]) << std::endl;
        for(unsigned int i=0; i<buffer_.size(); ++i){rec_buf.push_back(buffer_[i]);}
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
      std::vector<unsigned char> rec_buf;
      (*p_)(ec, rec_buf, n);
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

#endif
