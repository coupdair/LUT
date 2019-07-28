//CoolImage
#include "CImg.h"

//C++ base
#include <iostream>
#include <string>

//OpenMP
#include <omp.h>
#include <vector>

//! \todo [high] class: v baseOMPLock, v +print, v +access _ +progress, _ +buffer, v +run: acc, gen,store

//UDP
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/shared_ptr.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "UDP/allocator.hpp"

using namespace cimg_library;

#define VERSION "v0.1.2"

/*#define S 0 //sample*/

using boost::asio::ip::udp;

#include "UDP/yield.hpp"

boost::asio::io_service io_service(1);  ///!\ io_service is noncopyable, that means you should only refer to it via its address

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
#include "CDataReceiver.hpp"
#include "CDataStore.hpp"

CImg<unsigned int> copy(std::vector<unsigned char> *vec)
{
  std::cout << std::endl << "vec->size() : " << vec->size() << std::endl;
  CImg<unsigned int> result(vec->size(), 1, 1 , 1);
  for(unsigned int i=0; i < vec->size(); ++i)
  {
    result[i]=static_cast<unsigned int>((*vec)[i]);
  }
  result.print("CImgCopy",false);
  return result;
}


int main(int argc,char **argv)
{
  ///command arguments, i.e. CLI option
  cimg_usage(std::string("generate and store data.\n" \
  " It uses different GNU libraries (see --info option)\n\n" \
  " usage: ./store -h -I\n" \
  "        ./store -s 1024 -n 123 -X true -o samples/sample.png -p 1234\n" \
  "\n version: "+std::string(VERSION)+"\n compilation date:" \
  ).c_str());//cimg_usage

  const char* imagefilename = cimg_option("-o","samples/sample.png","output file name (e.g. \"-o data.png -d 3\" gives data_???.png)");
  const unsigned int digit=cimg_option("-d",6,  "number of digit for file names");
  const int width=cimg_option("-s",1024, "size   of udp buffer");
  //const int count=cimg_option("-n",123,  "number of vector");  //No count of the vectors because of the infinite loop
  const int nbuffer=cimg_option("-b",12, "size   of vector buffer (total size is b*s*4 Bytes)");
  const int threadCount=cimg_option("-c",0,"thread count");
  const unsigned short port=cimg_option("-p", 1234, "port where the packets are sent on the receiving device");
  //const bool spin=cimg_option("-sp", 1, "type of udp sending, possible values : {0 (block)|1 (spin)}");

  //définition en spin par défaut, block is not adequate for parallel threads

  ///standard options
  #if cimg_display!=0
  const bool show_X=cimg_option("-X",true,NULL);//-X hidden option
  bool show=cimg_option("--show",show_X,"show GUI (or -X option)");show=show_X|show;//same --show or -X option
  #endif
  const bool show_h   =cimg_option("-h",    false,NULL);//-h hidden option
        bool show_help=cimg_option("--help",show_h,"help (or -h option)");show_help=show_h|show_help; //same --help or -h option
  bool show_info=cimg_option("-I",false,NULL);//-I hidden option
  if( cimg_option("--info",show_info,"show compilation options (or -I option)") ) {show_info=true;cimg_library::cimg::info();}//same --info or -I option
  bool show_version=cimg_option("-v",false,NULL);//-v hidden option
  if( cimg_option("--version",show_version,"show version (or -v option)") ) {show_version=true;std::cout<<VERSION<<std::endl;return 0;}//same --version or -v option
  if(show_help) {/*print_help(std::cerr);*/return 0;}
  //}CLI option

  //OpenMP
  if(threadCount>0)
  {//user number of thread
    omp_set_dynamic(0);
    omp_set_num_threads(threadCount);
  }//user

  //OpenMP locks
  omp_lock_t print_lock;omp_init_lock(&print_lock);

  //! circular buffer
  CImgList<unsigned int> images(nbuffer,width,1,1,1);
  images[0].fill(0);
  images[0].print("image",false);
  //access locking
  omp_lock_t lck;omp_init_lock(&lck);

  //! access and status of buffer
  CImg<unsigned char> access(nbuffer,1,1,1);
  access.fill(0);//free
  access.print("access (free state)",false);fflush(stderr);

  //! generate data
  std::vector<omp_lock_t*> locks;locks.push_back(&print_lock);locks.push_back(&lck);
  CDataReceiver  receive(locks, port, width, &io_service);

  std::vector<unsigned char> rec_buf;//width);

  #pragma omp parallel shared(print_lock, access,images, receive)
  {
  int id=omp_get_thread_num(),tn=omp_get_num_threads();
  //std::cout << "id :" << id << ", tn : " << tn << std::endl;
  CDataStore     store(locks,imagefilename,digit);
  #pragma omp single
  {
  if(tn<2) {printf("error: run error, this process need at least 2 threads (presently only %d available)\n",tn);exit(2);}
  else {printf("info: running %d threads\n",tn);fflush(stdout);}
  }//single

  /*if(!spin)
  {
    io_service.run();		//not adequate for parallel threads
  }*/

  for(int n=0, i=0;;++i,++n)
  {
    switch(id)
    {
      case 0:
      {//generate
        receive.iteration(access,rec_buf, n,i, &io_service);
        std::cout << "\nrec_buf.size() receive : " << rec_buf.size() << "\n";
        images[n]=copy(&rec_buf);
        break;
      }//generate
      case 1:
      {//store
        std::cout << std::endl << "Size images before iter : " << images[n].size() << std::endl;
        store.iteration(access,images, n,i);
	//std::cout << std::endl << "Size images after iter : " << images[n].size() << std::endl;
        if(images[n].size()!=0)
        {
          std::cout << std::endl << "In store loop : value : " << images[n][0] << ", size : " << images[n].size() << std::endl;
        }
        break;
      }//store
    }//switch(id)
    //circular buffer
    if(n==nbuffer-1) n=-1;
  }//vector loop
  }//parallel section

  access.print("access (free state)",false);fflush(stderr);
  images.print("CImgList",false);
  return 0;
}//main

