//CoolImage
#include "CImg.h"

//C++ base
#include <iostream>
#include <string>
#include <vector>

//OpenMP
#include <omp.h>

#define VERSION "v0.2.7f"

#include "CDataStore.hpp"
#ifdef DO_GPU
//OpenCL
#include <boost/compute.hpp>
#include "CDataProcessorGPU.hpp"
#else
#include "CDataProcessor.hpp"
#endif //!DO_GPU
#include "CDataReceive.hpp"

using namespace cimg_library;
using boost::asio::ip::udp;

//types
//! thread access
typedef unsigned char Taccess;
//! data in
typedef unsigned int  Tdata;
//! data out
typedef unsigned int  Tdout;

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
  const char* resultfilename =cimg_option("-r","results/sample.png","result file name (e.g. \"-o proc.png -d 3\" gives proc_???.png)");
  const unsigned int digit=cimg_option("-d",6,  "number of digit for file names");
  const int width=cimg_option("-s",1024, "size   of udp buffer");
  const int count=cimg_option("-n",123,  "number of vector");  //! \todo [high] should be NO count of the vectors with an infinite loop (to implement)
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
  CImgList<Tdata> images(nbuffer,width,1,1,1);
  images[0].fill(0);
  images[0].print("image",false);
  //access locking
  omp_lock_t lck;omp_init_lock(&lck);
  //! result circular buffer
  CImgList<Tdata> results(nbuffer,width,1,1,1);
  results[0].fill(0);
  results[0].print("result",false);
  //accessR locking
  omp_lock_t lckR;omp_init_lock(&lckR);

  //! access and status of buffer
  CImg<Taccess> access(nbuffer,1,1,1);
  access.fill(0);//free
  access.print("access (free state)",false);fflush(stderr);
  //! access and status of Result buffer
  CImg<Taccess> accessR(nbuffer,1,1,1);
  accessR.fill(0);//free
  accessR.print("accessR (free state)",false);fflush(stderr);

  //! receive data
  std::vector<omp_lock_t*> locks;locks.push_back(&print_lock);locks.push_back(&lck);locks.push_back(&lckR);
  std::vector<omp_lock_t*> locksR;locksR.push_back(&print_lock);locksR.push_back(&lckR);

#ifdef DO_GPU
  //Choosing the target for OpenCL computing
  boost::compute::device gpu = boost::compute::system::default_device();
  #pragma omp parallel shared(print_lock, access,images, accessR,results, gpu)
#else
  #pragma omp parallel shared(print_lock, access,images, accessR,results)
#endif //!DO_GPU
  {
  int id=omp_get_thread_num(),tn=omp_get_num_threads();

  #pragma omp single
  {
  if(tn<4) {printf("error: run error, this process need at least 4 threads (presently only %d available)\n",tn);exit(2);}
  else {printf("info: running %d threads\n",tn);fflush(stdout);}
  }//single

  switch(id)
  {
    case 0:
    {//receive
      CDataReceive<Tdata> receive(locks, port, width, &io_service);
      receive.run(access,images, count);
      break;
    }//receive
    case 1:
    {//process
#ifdef DO_GPU
      CDataProcessorGPU<Tdata,Tdout> process(locks, gpu,width
#else
      CDataProcessor<Tdata,Tdout> process(locks
#endif //!DO_GPU
      , CDataAccess::STATUS_RECEIVED,CDataAccess::STATUS_PROCESSED //images
      , CDataAccess::STATUS_FREE,    CDataAccess::STATUS_FILLED    //results
      );
      process.run(access,images, accessR,results, count);
      break;
    }//process
    case 2:
    {//store
      CDataStore<Tdata> store(locks, imagefilename,digit, CDataAccess::STATUS_PROCESSED);//images
      store.run(access,images, count);
      break;
    }//store
    case 3:
    {//store
      CDataStore<Tdata> store(locksR, resultfilename,digit, CDataAccess::STATUS_FILLED);//results
      store.run(accessR,results, count);
      break;
    }//store
  }//switch(id)
  }//parallel section

  return 0;
}//main

