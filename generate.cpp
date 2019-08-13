//CoolImage
#include "CImg.h"

//C++ base
#include <iostream>
#include <string>

//OpenMP
#include <omp.h>
#include <vector>

//thread lock
#include "CDataGenerator.hpp"
#include "CDataSender.hpp"

//! \todo [high] class: v baseOMPLock, v +print, v +access _ +progress, _ +buffer, v +run: acc, gen,store

using namespace cimg_library;

#define VERSION "v0.2.2d"

#define S 0 //sample

//Function to copy the data in a CImg in a vector
std::vector<unsigned char> copy(CImg<unsigned int> img)
{
  std::vector<unsigned char> result;
  for(int i=0; i<img.width(); ++i)
  {
    result.push_back(static_cast<unsigned char>(img[i]));
  }
  return result;
}


int main(int argc,char **argv)
{
  ///command arguments, i.e. CLI option
  cimg_usage(std::string("generate and send data via UDP.\n" \
  " It uses different GNU libraries (see --info option)\n\n" \
  " usage: ./generate -h\n" \
  "        ./generate -s 1024 -n 123 -X true -p 1234 -i 10.10.15.1 -w 1234657\n" \
  "\n version: "+std::string(VERSION)+"\n compilation date:" \
  ).c_str());//cimg_usage

  //const char* imagefilename = cimg_option("-o","sample.cimg","output file name (e.g. \"-o data.cimg -d 3\" gives data_???.cimg)");
  //const int digit=cimg_option("-d",6,  "number of digit for file names");
  const int width=cimg_option("-s",1024, "size   of udp buffer");
  const int count=cimg_option("-n",256,  "number of frames");
  const int nbuffer=cimg_option("-b",12, "size   of vector buffer (total size is b*s*4 Bytes)");
  const int threadCount=cimg_option("-c",2,"thread count");
  const unsigned short port=cimg_option("-p",1234,"port where the packets are send on the receiving device");
  const std::string ip=cimg_option("-i", "10.10.15.1", "ip address of the receiver");
  const int twait=cimg_option("-w", 123456789, "waiting time between udp frames");

  //conversion of twait into a boost::uint64_t
  const boost::uint64_t wait=static_cast<std::size_t>(twait);

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

  //UDP_buffer initialization
  std::vector<unsigned char> write_buf(width);
  //////////TODO : no UDP vector, data directly collected from the circular buffer///////////////

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

  #pragma omp parallel shared(print_lock, access,images)
  {
  int id=omp_get_thread_num(),tn=omp_get_num_threads();
  CDataGenerator generate(locks);
  CDataSender    send(locks,ip,port);

  #pragma omp single
  {
  if(tn<2) {printf("error: run error, this process need at least 2 threads (presently only %d available)\n",tn);exit(2);}
  else {printf("\ninfo: running %d threads\n",tn);fflush(stdout);}
  }//single

  //for(int n=0,i=0;i<count;++i,++n)
  for(int n=0,i=0;i<count;++i,++n)
  {
    switch(id)
    {
      case 0:
      {//generate
        generate.iteration(access,images, n,i);
        break;
      }//generate
      case 1:
      {//send
	write_buf=copy(images[n]);
//      copy(images[n].begin(), images[n].end(), back_inserter(write_buf));		//Ne fait pas ce qui est demandé
        send.iteration(access,write_buf, n,i, wait);
        break;
      }//send
    }//switch(id)
    //circular buffer
    if(n==nbuffer-1) n=-1;
  }//vector loop
  }//parallel section

  access.print("access (free state)",false);fflush(stderr);
  images.print("CImgList",false);
  return 0;
}//main

